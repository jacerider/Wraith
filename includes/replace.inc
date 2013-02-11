<?php

function wraith_js_alter_replace(&$js){

  $path = drupal_get_path('theme', 'wraith').'/assets';

  // Make sure we inject either the minified or uncompressed version as desired.
  $min = theme_get_setting('jquery_update_compression_type') ? theme_get_setting('jquery_update_compression_type') : 'min';
  $min = $min == 'none' ? '' : '.min';
  $cdn = theme_get_setting('jquery_update_jquery_cdn') ? theme_get_setting('jquery_update_jquery_cdn') : 'none';

  // Replace jQuery with the latest version.
  $version = theme_get_setting('jquery_update_jquery_version') ? theme_get_setting('jquery_update_jquery_version') : '1.7';
  wraith_jquery_replace($js, $cdn, $path, $min, $version);

  // Replace jQuery UI with CDN or local files. If from a CDN include all of jQuery UI.
  wraith_jqueryui_replace($js, $cdn, $path, $min);

  // Replace the jQuery Cookie plugin.
  if(isset($js['misc/jquery.cookie.js'])) $js['misc/jquery.cookie.js']['data'] = $path . '/replace/ui/external/jquery.cookie.js';
  // Noting the version based on git commit as no version number is available.
  if(isset($js['misc/jquery.cookie.js'])) $js['misc/jquery.cookie.js']['version'] = '67fb34f6a866c40d0570';

  // Replace jQuery Form plugin.
  if(isset($js['misc/jquery.form.js'])){
    $js['misc/jquery.form.js']['data'] = $path . '/replace/misc/jquery.form' . $min . '.js';
    $js['misc/jquery.form.js']['version'] = '2.69';
  }

  // Replace files for jQuery 1.7 and up
  if (isset($js['misc/states.js']) && version_compare($version, '1.7', '>=')) {
    $js['misc/states.js']['data'] = $path . '/replace/misc/1.7/states.js';
  }
}

/**
 * Update jQuery to the CDN or local path.
 *
 * @param array $js
 *   The library definition array as seen in hook_library_alter().
 * @param string $cdn
 *   The name of the CDN option to use. Possible options are:
 *   - none
 *   - google
 *   - microsoft
 * @param string $version
 *   The version of jQuery to use.
 */
function wraith_jquery_replace(&$js, $cdn, $path, $min, $version) {
  // Make sure to use the latest version in given branch.
  $trueversion = NULL;
  switch ($version) {
    case '1.5':
      $trueversion = '1.5.2';
      break;
    case '1.7':
      $trueversion = '1.7.1';
      break;
    case '1.8':
      $trueversion = '1.8.2';
      break;
  }
  $js['misc/jquery.js']['version'] = $trueversion;

  // Check for CDN support.
  switch($cdn) {
    case 'google':
      $js['misc/jquery.js']['data'] = 'https://ajax.googleapis.com/ajax/libs/jquery/'. $trueversion . '/jquery' . $min . '.js';
      $js['misc/jquery.js']['type'] = 'external';
      // wraith_jquery_backup($js, $path, $min, $version);
      break;
    case 'microsoft':
      $js['misc/jquery.js']['data'] = 'http://ajax.aspnetcdn.com/ajax/jQuery/jquery-'. $trueversion . $min . '.js';
      $js['misc/jquery.js']['type'] = 'external';
      // wraith_jquery_backup($js, $path, $min, $version);
      break;
    case 'jquery':
      $js['misc/jquery.js']['data'] = 'http://code.jquery.com/jquery-'. $trueversion . $min . '.js';
      $js['misc/jquery.js']['type'] = 'external';
      // wraith_jquery_backup($js, $path, $min, $version);
      break;
    case 'none':
    default:
      $js['misc/jquery.js']['data'] = $path . '/replace/jquery/'. $version . '/jquery' . $min . '.js';
      break;
  }
}

/**
 * Add the local fallback in case jQuery from the CDN is unavailable.
 *
 * @param array $js
 *   The $libraries array as seen in hook_library_alter()
 * @param string $path
 *   The path to the module where replacements can be found.
 * @param string $min
 *   The '.min' to include in the file name if we are requesting a minified version.
 * @param string $version
 *   The verison of jQuery to use.
 */
// function wraith_jquery_backup(&$js, $path, $min, $version) {
//   $js[] = array(
//     'dsmata' => 'window.jQuery || document.write("<script src=\'' . base_path() . $path . '/replace/jquery/'. $version . '/jquery' . $min . '.js\'>\x3C/script>")',
//     'type' => 'inline',
//     'group' => JS_LIBRARY,
//     'weight' => -19.999999999,
//   );
// }

/**
 * Update jQuery UI to the CDN or local path.
 *
 * @param array $js
 *   The library definition array as seen in hook_library_alter().
 * @param string $cdn
 *   The name of the CDN option to use. Possible options are:
 *   - none
 *   - google
 *   - microsoft
 */
function wraith_jqueryui_replace(&$js, $cdn, $path, $min) {
  // Return if not needed
  if(empty($js['misc/ui/jquery-ui'.$min.'.js'])) return;

  // Replace all CSS files.
  $names = drupal_map_assoc(array(
    'ui.accordion', 'ui.autocomplete', 'ui.button', 'ui.datepicker',
    'ui.dialog', 'ui.progressbar', 'ui.resizable', 'ui.selectable',
    'ui.slider', 'ui.tabs',
  ));
  $names['ui'] = 'ui.core';
  $csspath = $path . '/replace/ui/themes/base/' . (($min == '.min') ? 'minified/' : '');
  foreach ($names as $name => $file) {
    if(!isset($js["misc/ui/jquery.$file.css"])) continue;
    $js["misc/ui/jquery.$file.css"]['data'] = $csspath . 'jquery.' . $file . $min . '.css';
  }
  // Make sure ui.theme is replaced as well.
  if(!empty($js['misc/ui/jquery.ui.theme.css'])) $js['misc/ui/jquery.ui.theme.css']['data'] = $csspath . 'jquery.ui.theme' . $min . '.css';

  // Replace jQuery UI's JavaScript, beginning by defining the mapping.
  $names = drupal_map_assoc(array(
    'ui.accordion', 'ui.autocomplete', 'ui.button', 'ui.datepicker',
    'ui.dialog', 'ui.draggable', 'ui.droppable', 'ui.mouse', 'ui.position',
    'ui.progressbar', 'ui.resizable', 'ui.selectable', 'ui.slider',
    'ui.sortable', 'ui.tabs', 'ui.widget', 'effects.blind', 'effects.bounce',
    'effects.clip', 'effects.drop', 'effects.explode', 'effects.fade',
    'effects.fold', 'effects.highlight', 'effects.pulsate', 'effects.scale',
    'effects.shake', 'effects.slide', 'effects.transfer',
  ));
  $names['ui'] = 'ui.core';
  $names['effects'] = 'effects.core';

  switch($cdn) {
    case 'google':
      $cdn = 'https://ajax.googleapis.com/ajax/libs/jqueryui/1.8.11/jquery-ui' . $min . '.js';
      wraith_jqueryui_cdn($cdn, $js, $path, $min, $names);
      // wraith_jqueryui_backup($js, $path, $min);
      break;
    case 'microsoft':
      $cdn = 'http://ajax.aspnetcdn.com/ajax/jquery.ui/1.8.11/jquery-ui' . $min . '.js';
      wraith_jqueryui_cdn($cdn, $js, $path, $min, $names);
      // wraith_jqueryui_backup($js, $path, $min);
      break;
    case 'none':
      wraith_jqueryui_local($js, $path, $min, $names);
      break;
  }
}

/**
 * Add the local fallback in case jQuery UI from the CDN is unavailable.
 *
 * @param array $js
 *   The $libraries array as seen in hook_library_alter()
 * @param string $path
 *   The path to the module where replacements can be found.
 * @param string $min
 *   The '.min' to include in the file name if we are requesting a minified version.
 */
// function wraith_jqueryui_backup(&$js, $path, $min) {
//   $js_path = ($min == '.min') ? '/replace/ui/ui/minified/jquery-ui.min.js' : '/replace/ui/ui/jquery-ui.js';
//   $js[] = array(
//     'data' => 'window.jQuery.ui || document.write("<script src=\'' . base_path() . $path . $js_path . '\'>\x3C/script>")',
//     'type' => 'inline',
//     'group' => JS_LIBRARY,
//     'weight' => -10.999999999,
//   );
// }

/**
 * Handle when jQuery UI is updated to the cdn version.
 *
 * @param array $js
 *   The $libraries array as seen in hook_library_alter()
 * @param string $path
 *   The path to the module where replacements can be found.
 * @param string $min
 *   The '.min' to include in the file name if we are requesting a minified version.
 * @param array $names
 *   An array mapping jquery ui parts to their file names.
 */
function wraith_jqueryui_cdn($cdn, &$js, $path, $min, $names) {
  // Construct the jQuery UI path and replace the JavaScript.
  $jspath = $path . '/replace/ui/ui/' . ($min == '.min' ? 'minified/' : '');
  foreach ($names as $name => $file) {
    $corefile = 'misc/ui/jquery.' . $file . '.min.js';
    if(!isset($js[$corefile])) continue;
    // Remove the core files.
    unset($js[$corefile]);
    $js[$name]['version'] = '1.8.11';
  }

  // UI is used by all of UI. Add the js cdn here.
  $js[$cdn] = array(
    'data' => $cdn,
    'type' => 'external',
    'group' => JS_LIBRARY,
    'weight' => -11,
  );
}

/**
 * Handle when jQuery UI is updated to the local version.
 *
 * @param array $js
 *   The $libraries array as seen in hook_library_alter()
 * @param string $path
 *   The path to the module where replacements can be found.
 * @param string $min
 *   The '.min' to include in the file name if we are requesting a minified version.
 * @param array $names
 *   An array mapping jquery ui parts to their file names.
 */
function wraith_jqueryui_local(&$js, $path, $min, $names) {

  // Construct the jQuery UI path and replace the JavaScript.
  $jspath = $path . '/replace/ui/ui/' . ($min == '.min' ? 'minified/' : '');
  foreach ($names as $name => $file) {
    $corefile = 'misc/ui/jquery.' . $file . '.min.js';
    if(!isset($js[$corefile])) continue;
    $js[$name]['js'][$corefile]['data'] = $jspath . 'jquery.' . $file . $min . '.js';
    $js[$name]['version'] = '1.8.11';
  }
}
