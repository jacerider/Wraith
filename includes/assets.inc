<?php

/**
 * Implements hook_js_alter().
 *
 * This function checks all JS files currently added via drupal_add_js() and
 * checks to see if any of them are disabled via theme settings.
 */
function wraith_js_alter(&$js) {

  // Replace jQuery
  require_once drupal_get_path('theme', 'wraith') . '/includes/replace.inc';
  wraith_js_alter_replace($js);

  if (theme_get_setting('wraith_coffee')) {
    require_once drupal_get_path('theme', 'wraith') . '/includes/coffee.inc';
    wraith_aggregate_coffee($js);
  }
}


/**
 * Implements hook_css_alter().
 *
 * This function checks all CSS files currently added via drupal_add_css() and
 * checks to see if any of them are disabled via theme settings and if a
 * direction-specific file should be included.
 */
function wraith_css_alter(&$css) {
  global $language;

  // Disable core and modules CSS via theme settings
  if (theme_get_setting('wraith_enable_css')) {
    $enable_files = wraith_get_assets_list('css');
    // remove selected css files
    if ($modules = theme_get_setting('wraith_enable_css_modules')) {
      global $theme_key;
      // Let all styles from the current theme through
      $modules += array($theme_key=>$theme_key);
      $enable_files = wraith_get_assets_list('css', NULL, $modules);
    }
    if (is_array(theme_get_setting('wraith_enable_css_files')) && $files = array_filter(theme_get_setting('wraith_enable_css_files'))) {
      $enable_files = array_diff_key($enable_files, $files);
    }
    $css = array_diff_key($css, $enable_files);
  }
  elseif (theme_get_setting('wraith_disable_css')) {
    $disable_files = array();
    if (theme_get_setting('wraith_disable_css_all')) {
      // remove all css files from contrib and core modules
      $disable_files = wraith_get_assets_list();
    }
    else {
      // remove selected css files
      if ($modules = theme_get_setting('wraith_disable_css_modules')) {
        $disable_files = array_merge($disable_files, wraith_get_assets_list('css', $modules));
      }
      if (is_array(theme_get_setting('wraith_disable_css_files')) && $files = array_filter(theme_get_setting('wraith_disable_css_files'))) {
        $disable_files = array_merge($disable_files, $files);
      }
    }

    $css = array_diff_key($css, $disable_files);
  }

  foreach ($css as $data => &$item) {
    // The CSS_SYSTEM aggregation group doesn't make any sense. Therefore, we are
    // pre-pending it to the CSS_DEFAULT group. This has the same effect as giving
    // it a separate (low-weighted) group but also allows it to be aggregated
    // together with the rest of the CSS.
    if ($item['group'] == CSS_SYSTEM) {
      $item['group'] = CSS_DEFAULT;
      $item['weight'] = $item['weight'] - 100;
    }

    // Remove any SCSS files at this point as Sonar should have already taken
    // care of them. If Sonar is not installed this will prevent them from
    // causing issues.
    $extension = drupal_substr($data, -4);
    if (in_array($extension, array('scss', 'sass'))) {
      unset($css[$data]);
    }
  }
}

/**
 * Returns a list of Bootstrap SCSS components
 */
function wraith_get_bootstrap_scss($options = FALSE) {
  $return = array();
  $files = array(
    // Core variables and mixins
    '_variables',
    '_mixins',
    // CSS Reset
    '_normalize',
    '_print',
    // Core CSS
    '_scaffolding',
    '_type',
    '_code',
    '_grid',
    '_tables',
    '_forms',
    '_buttons',
    // Components
    '_component-animations',
    '_glyphicons',
    '_dropdowns',
    '_button-groups',
    '_input-groups',
    '_navs',
    '_navbar',
    '_breadcrumbs',
    '_pagination',
    '_pager',
    '_labels',
    '_badges',
    '_jumbotron',
    '_thumbnails',
    '_alerts',
    '_progress-bars',
    '_media',
    '_list-group',
    '_panels',
    '_wells',
    '_close',
    // Components w/ JavaScript
    '_modals',
    '_tooltip',
    '_popovers',
    '_carousel',
    // Utility classes
    '_utilities',
    '_responsive-utilities',
  );
  foreach($files as $s){
    $label = ucwords(str_replace('_', '', str_replace('-', ' ', $s)));
    $return[$s] = $label;
  }
  return $return;
}

/**
 * Returns a list of Bootstrap Javascript components
 */
function wraith_get_bootstrap_js($options = FALSE) {
  $return = array();
  $files = array(
    'transition',
    'affix',
    'alert',
    'button',
    'carousel',
    'collapse',
    'dropdown',
    'modal',
    'scrollspy',
    'tab',
    'tooltip',
    'popover'
  );
  foreach($files as $s){
    $label = ucwords(str_replace('_', '', str_replace('-', ' ', $s)));
    $return[$s] = $label;
  }
  return $return;
}

/**
 * Include bootstrap scss files
 */
function wraith_bootstrap_include(){
  global $theme_key;
  $scss = theme_get_setting('wraith_bootstrap_scss');
  $path = drupal_get_path('theme', 'wraith').'/assets/bootstrap/stylesheets';
  $override = drupal_get_path('theme', $theme_key).'/assets/sass/override';
  $options = array(
    'every_page' => TRUE,
    'weight' => -1000
  );
  if(is_array($scss)){
    foreach($scss as $s){
      if(empty($s)) continue;
      $use_path = $path;
      // Allow local overriding of Bootstrap files
      if(file_exists($override.'/'.$s.'.scss')){
        $use_path = $override;
      }
      drupal_add_css($use_path.'/'.$s.'.scss', $options);
    }
  }
  $js = theme_get_setting('wraith_bootstrap_js');
  $path = drupal_get_path('theme', 'wraith').'/assets/bootstrap/javascripts';
  $options = array(
    'every_page' => TRUE,
  );
  if(is_array($js)){
    foreach($js as $s){
      if(empty($s)) continue;
      drupal_add_js($path.'/'.$s.'.js', $options);
    }
  }
}

/**
 * Include theme scss files
 */
function wraith_scss_include(){
  $path = drupal_get_path('theme', 'wraith').'/assets';
  $options = array(
    'every_page' => TRUE,
  );
  foreach(wraith_get_scss_list() as $s){
    drupal_add_css($s, $options);
  }
}

/**
 * Includes all custom style sheets for the current theme.
 */
function wraith_css_include() {

  $dynasty = wraith_theme_dynasty();

  foreach ($dynasty as $theme) {
    $info = drupal_parse_info_file($theme->filename);

    if (isset($info['styles']) && !empty($info['styles'])) {
      foreach ($info['styles'] as $file => $style) {
        $file = drupal_get_path('theme', $theme->name) . "/{$file}";
        $style['options']['theme'] = $theme->name;
        drupal_add_css($file, $style['options']);
      }
    }
  }
}

/**
 * Build the theme tree from base theme to active theme.
 */
function wraith_theme_dynasty() {
  global $theme_key;
  $themes = list_themes();
  $dynasty = array();
  $dynasty[] = $obj = $themes[$theme_key];

  while (isset($obj->base_theme) && isset($themes[$obj->base_theme]) && !empty($themes[$obj->base_theme])) {
    $dynasty[] = $obj = $themes[$obj->base_theme];
  }

  return array_reverse($dynasty);
}

/**
 * Returns a list of modules
 */
function wraith_get_modules_list($options = FALSE) {
  $modules = $options ? system_list('module_enabled') : module_list();
  asort($modules);

  return $modules;
}

/**
 * Returns a list of assets (js/css)
 */
function wraith_get_assets_list($type = 'css', $filter = array(), $exclude = array()) {
  $cache = &drupal_static(__FUNCTION__);
  $cid = '';
  if(is_array($filter) && count($filter)) $cid .= implode('', array_keys($filter));
  if(is_array($exclude) && count($exclude)) $cid .= implode('', array_keys($exclude));
  $cid = $type.substr(drupal_hash_base64($cid), 0, 25);
  if (!isset($cache[$cid])) {
    $files = array();
    $enabled_modules = wraith_get_modules_list(TRUE);

    if (is_array($filter) && count($filter)) {
      $filter = array_combine($filter, $filter);
      $modules = array_intersect_key($enabled_modules, $filter);
    }
    elseif ($filter == 'jquery' && isset($modules['jquery'])) {
      $modules['jquery']->filename = 'misc/jquery.js';
    }
    elseif ($filter == 'wraith') {
      $modules['wraith']->filename = drupal_get_path('theme', 'wraith') . '/assets/scripts/wraith.js';
    }
    else {
      $modules = $enabled_modules;
      $modules += list_themes();
    }

    if(is_array($exclude) && count($exclude)){
      $filter = array_combine($exclude, $exclude);
      $modules = array_diff_key($modules, $filter);
    }

    foreach ($modules as $name => $module) {
      $module_path = pathinfo($module->filename, PATHINFO_DIRNAME);
      $module_assets_files = file_scan_directory($module_path, '/.*\.' . $type . '$/');
      foreach ($module_assets_files as $path => $file) {
        $files[$path] = $path;
      }
    }

    $module_assets_files = file_scan_directory('misc', '/.*\.' . $type . '$/');
    foreach ($module_assets_files as $path => $file) {
      $files[$path] = $path;
    }

    $cache[$type] = $files;
  }

  return $cache[$type];
}

/**
 * Include theme scss files
 */
function wraith_get_scss_list(){
  $path = drupal_get_path('theme', 'wraith').'/assets/sass';
  $scss = array();
  foreach(file_scan_directory($path, "/^.*\.(scss)$/i") as $s){
    $scss[] = $s->uri;
  }
  return $scss;
}

/**
 * Get all scss files currently used on the page
 */
function wraith_get_scss_all(){
  $css = drupal_add_css();
  return wraith_pick_files($css);
}

/**
 * A callback to remove all core Wraith files from wraith_get_scss_all
 */
function wraith_remove_wraith_files($scss){
  $path = drupal_get_path('theme', 'wraith');
  return strpos($scss['data'],$path) !== 0;
}
