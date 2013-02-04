<?php

/**
 * @file
 * Handles compiling of .sass / .scss files.
 */

// Composer autoload
require_once drupal_get_path('theme', 'wraith') . '/includes/assetic/vendor/autoload.php';

// Manage filters
use Assetic\Asset\FileAsset;
use Assetic\Filter\CompassFilter;

/**
 * Implementation of hook_element_info_alter().
 */
function wraith_element_info_alter(&$type) {

  // Add wraith_pre_render() to be invoked by drupal_render().
  //array_unshift($type['styles']['#pre_render'], 'wraith_pre_render');

  // Show recompilation message once an hour.
  if (theme_get_setting('wraith_sass_recompile') && user_access('administer site configuration') && flood_is_allowed('wraith_sass_recompile_warning', 1)) {
    flood_register_event('wraith_sass_recompile_warning');
    drupal_set_message(t('Your Sass files are being recompiled on every page request. Don\'t forget to !disable before going public.', array('!disable' => l(t('disable this feature'),'admin/appearance/settings/' . variable_get('theme_default','none')))), 'warning');
  }
  // Show development message once a day.
  if (theme_get_setting('wraith_devel') && user_access('administer site configuration') && flood_is_allowed('wraith_devel_warning', 1, 86400)) {
    flood_register_event('wraith_devel_warning');
    drupal_set_message(t('Your theme has development mode on. For better performance (smaller CSS files), don\'t forget to !disable before going public.', array('!disable' => l(t('disable this feature'), 'admin/appearance/settings/' . variable_get('theme_default','none')))), 'warning');
  }
}

/**
 * Get global sass file information
 */
function wraith_sass_global_info($files = array()){
  global $theme_key;
  $info = &drupal_static(__FUNCTION__);
  if (!isset($info) && is_array($files) && !empty($files)) {
    $cid = implode('', array_keys($files));
    $info['cid'] = $theme_key.'-'.substr(drupal_hash_base64($cid), 0, 25);
    $files_directory = variable_get('file_' . file_default_scheme() . '_path', conf_path() . '/files') . '/wraith/' . $theme_key . '/stylesheets';
    $info['directory'] = theme_get_setting('wraith_compiler_destination') ? theme_get_setting('wraith_compiler_destination') . '/' . $theme_key . '/stylesheets' : $files_directory;
    $info['path'] = $info['directory'] . '/' . $info['cid'] . '.css';
    $info['files'] = drupal_map_assoc(array_keys($files));
  }
  return $info;
}

/**
 * Builds the Sass cache.
 * Added by wraith_element_info_alter() to be invoked by drupal_render().
 *
 * @param $elements
 * A render array containing:
 * '#items': The CSS items as returned by drupal_add_css() and altered by
 * drupal_get_css().
 * '#group_callback': A function to call to group #items to enable the use of
 * fewer tags by aggregating files and/or using multiple @import statements
 * within a single tag.
 * '#aggregate_callback': A function to call to aggregate the items within the
 * groups arranged by the #group_callback function.
 * @param $clean
 * Force a rebuild if TRUE
 *
 * @return $elements
 * The modified (pre-rendered) $elements parameter.
 */
function wraith_sass_alter(&$css) {
  global $theme_key;
  $files = wraith_pick_files($css);
  uasort($files, 'drupal_sort_weight');

  // We can bail out here if there are no Sass files anyways.
  if (empty($files)) {
    // Remove the files from the array of stylesheets.
    $css = array_diff_key($css, $files);
    return $elements;
  }

  $data = array();

  // Get data required for compiled sass name
  $info = wraith_sass_global_info($files);
  unset($info['files']);
  extract($info);

  // Get cached data
  $cached = cache_get($cid, 'cache_page');

  // Initial checks to see if we should compile
  // - Does the compiled file exist?
  // - Do we have cached data of this file?
  // - Have we force set any file to be recompiled?
  $compile = !file_exists($path) || empty($cached->data) ? TRUE : FALSE;
  // If create is still set to false, lets check to see if any of the files
  // have been updated since our last compile
  if(!$compile){
    foreach ($files as $key => $file) {
      if (file_exists($css[$key]['data'])) {
        if($file['recompile'] || (filemtime($path) < filemtime($file['data']))){
          $compile = TRUE;
        }
      }else{
        unset($css[$key]);
      }
    }
  }

  // Loop through each file again and remove from $elements array
  // and read if we are compiling
  foreach ($files as $key => $file) {
    if (file_exists($css[$key]['data'])) {
      // We recompile this file if the file doesn't exist, if recompile equals true,
      // if cache was cleared or if the file have changed since it was last compiled.
      if ($compile){
        // Read the file.
        $data[$key] = wraith_load_stylesheet($file['data']);
      }
      // Remove this element as it will be combined intoa single element below
      unset($css[$key]);
    }
  }

  // Update file data
  $file['data'] = $path;
  $file['theme'] = $theme_key;
  $file['wraith_alter'] = TRUE;

  // Update the item in the stylesheets array.
  $css[$path] = $file;
  $css[$path]['data'] = $path;

  if(!empty($data)){
    // Compile.
    wraith_parse($data, $file, $file['syntax'], $file['theme']);
    cache_set($cid, time(), 'cache_page', CACHE_TEMPORARY);
  }

  // Reset "compile_for_production" when finished compiling.
  if (!empty($_SESSION['wraith_compile_for_production'])) {
    $_SESSION['wraith_compile_for_production'] = FALSE;
  }
}

/**
 * Picks all Sass files from an array of stylesheets.
 *
 * @param $items
 *   An array of stylesheets.
 *
 * @return
 *   The extracted files as an array.
 */
function wraith_pick_files($items) {
  $files = &drupal_static(__FUNCTION__);
  if (!isset($files)){
    $files = array();
    foreach ($items as $key => $file) {
      if ($file['type'] == 'file') {
        $extension = drupal_substr($file['data'], -5);
        if (in_array($extension, array('.scss', '.sass'))) {
          $file['syntax'] = $extension == '.scss' ? 'scss' : 'sass';
          $file['recompile'] = isset($file['recompile']) ? $file['recompile'] : FALSE;
          // If the file is set to recompile on every page load then we don't want it
          // to be aggregated, this will also put them in a <link> tag while developing.
          if (theme_get_setting('wraith_devel') || $file['recompile']) {
            $file['preprocess'] = FALSE;
          }
          $files[$key] = $file;
        }
      }
    }
  }
  return $files;
}

/**
 * Loads a stylesheet and writes the base path to all url declarations.
 *
 * @param $file
 *   A filepath or an array representing a stylesheet.
 *
 * @return
 *   A string that represents the processed contents of the stylesheet.
 */
function wraith_load_stylesheet($file) {
  $data =  file_get_contents($file);
  // Build the base URL of this CSS file. Start with the full URL.
  $base = file_create_url($file);
  // Move to the parent.
  $base = substr($base, 0, strrpos($base, '/'));
  // Simplify to a relative URL if the stylesheet URL starts with the base URL
  // of the website.
  if (substr($base, 0, strlen($GLOBALS['base_root'])) == $GLOBALS['base_root']) {
    $base = substr($base, strlen($GLOBALS['base_root']));
  }
  _drupal_build_css_path(NULL, $base . '/');

  if (theme_get_setting('wraith_url_rewrite')) {
    // Anchor all paths in the CSS with its base URL, ignoring external, absolute
    // paths, and compass url helper functions.
    $data = preg_replace_callback('/(?<!-)url\(\s*[\'"]?(?![a-z]+:|\/+)([^\'")]+)[\'"]?\s*\)/i', '_drupal_build_css_path', $data);
  }
  return $data;
}

/**
 * Parse a SCSS string and transform it into CSS.
 *
 * @param $data
 *   A SCSS string.
 * @param $file
 *   The SASS or SCSS file that $data belongs to described by an array.
 * @param $syntax
 *   The syntax (SASS or SCSS) of the file contents. This information is needed
 *   by the parser.
 * @param $theme_name
 *   String specifying the theme name.
 *
 * @return
 *   The transformed CSS as a string.
 */
function wraith_parse($data, $file, $syntax, $theme_name) {

  $images_path = theme_get_setting('wraith_images_path') ? theme_get_setting('wraith_images_path') : drupal_get_path('theme', $theme_name) . '/assets/images';
  $fonts_path = theme_get_setting('wraith_fonts_path') ? theme_get_setting('wraith_fonts_path') : drupal_get_path('theme', $theme_name) . '/assets/fonts';
  $stylesheets_path = drupal_get_path('theme', $theme_name) . '/stylesheets';
  $files_directory = variable_get('file_' . file_default_scheme() . '_path', conf_path() . '/files') . '/wraith/' . $theme_name;

  // Watch the theme dynasty for partials
  $dynasty = wraith_theme_dynasty();
  $load_paths = array();
  foreach ($dynasty as $theme) {
    $load_paths[] = DRUPAL_ROOT . '/' . drupal_get_path('theme', $theme->name) . '/sass/';
    $load_paths[] = DRUPAL_ROOT . '/' . drupal_get_path('theme', $theme->name) . '/stylesheets/';
  }

  // Sass options
  $options = array(
    'theme' => $theme_name,
    'style' => !theme_get_setting('wraith_devel') || !empty($_SESSION['wraith_compile_for_production']) ? 'compressed' : 'expanded',
    'syntax' => $syntax,
    'directory' => theme_get_setting('wraith_compiler_destination') ? theme_get_setting('wraith_compiler_destination') . '/' . $theme_name : $files_directory,
    'load_paths' => $load_paths,
    'debug_info' => theme_get_setting('wraith_devel'),
    'no_line_comments' => theme_get_setting('wraith_devel') ? false : true,
    'compass_path' => variable_get('wraith_compass_path', 'compass'),
    'http_images_path'=> '/' . $images_path,
    'images_path'=> DRUPAL_ROOT . '/' . $images_path,
    'fonts_path'=> DRUPAL_ROOT . '/' . $fonts_path,
    'css_path'=> DRUPAL_ROOT . '/' . $stylesheets_path,
  );

  // Alter SASS just before it is sent to the parser
  // @see hook_wraith_alter().
  if (!empty($file['wraith_alter'])) {
    drupal_alter('wraith', $data, $theme_name);
  }

  $data = wraith_parse_compass($data, $file, $options);

  return $data;
}

/**
 * Parse a SCSS string and transform it into CSS using Compass compiler.
 *
 * @param $data
 *   A SCSS string.
 * @param $file
 *   The SASS or SCSS file that $data belongs to described by an array.
 * @param $options
 *   Array containg all information needed by the compiler.
 * @return
 *   The transformed CSS as a string.
 */
function wraith_parse_compass($data, $file, $options) {
  $directory = $options['directory'] . '/stylesheets/';
  $sass_filepath = $directory . 'tmp.' . basename($file['data']);
  $css_filepath = $directory . str_replace($options['syntax'], 'css', basename($file['data']));
  $syntax = $options['syntax'] == 'scss' ? true : false;

  // Put $data in a file that Compass can compile.
  if (!file_prepare_directory($directory, FILE_CREATE_DIRECTORY)) {
    drupal_set_message(t('The directory !directory could not be created or is not writable. Please consult your !watchdog for a detailed error description.', array('!directory' => $directory, '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');
  };
  if (!file_unmanaged_save_data($data, $sass_filepath, FILE_EXISTS_REPLACE)) {
    drupal_set_message(t('The file <code>!file</code> could not be created. Please consult your !watchdog for a detailed error description.', array('!file' => $sass_filepath, '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');

  };
  // Let assetic compile it
  $asset = new FileAsset($sass_filepath,
    array(
      $compass = new CompassFilter($options['compass_path']),
    )
  );

  // Compass settings
  $compass->setScss($syntax);
  $compass->setDebugInfo($options['debug_info']);
  $compass->setStyle($options['style']);
  $compass->setNoLineComments($options['no_line_comments']);
  $compass->setLoadPaths(array_reverse($options['load_paths']));
  $compass->setHttpPath('/');
  $compass->setHttpImagesPath($options['http_images_path']);
  $compass->setImagesDir($options['images_path']);
  $compass->setGeneratedImagesPath(DRUPAL_ROOT . '/' . $options['directory'] . '/images/');
  $compass->setHttpGeneratedImagesPath('/' . $options['directory'] . '/images/');

  try {
    $data = $asset->dump();
    // If no exceptions were thrown, try saving the result to a CSS file.
    if (!file_unmanaged_save_data($data, $css_filepath, FILE_EXISTS_REPLACE)) {
      drupal_set_message(t('The file <code>!file</code> could not be created. Please consult your !watchdog for a detailed error description.', array('!file' => $css_filepath, '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');
    };
    file_unmanaged_delete($sass_filepath);

    return $data;
  }
  catch (Exception $e) {
    // If compilation failed.
    drupal_set_message(t('Could not compile the file <code>!file</code>. Please consult your !watchdog for a detailed error description.', array('!file' => basename($file['data']), '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');
    watchdog('wraith', '<pre>' . $e->getMessage() . '</pre>', NULL, WATCHDOG_DEBUG, NULL);
  }
}
