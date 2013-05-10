<?php

// Composer autoload
require_once drupal_get_path('theme', 'wraith') . '/includes/assetic/vendor/autoload.php';

// Manage filters
use Assetic\Asset\AssetCollection;
use Assetic\Asset\FileAsset;
// use Assetic\Filter\PackerFilter;

function wraith_aggregate_js(&$js){
  $files = &drupal_static(__FUNCTION__);
  if (!isset($files)) {
    $files = theme_get_setting('wraith_aggregate_js_files');

    if(empty($files)){
      $files = $js;
      return;
    }

    // We want to remove all files set to be included globally
    $files = drupal_map_assoc($files);

    // Get new paths for any of our files
    // This takes any files altered by jquery_update module or this theme
    foreach($files as $file){
      if(isset($js[$file])) $files[$file] = $js[$file]['data'];
    }

    unset($files[0]);
    $js = array_diff_key($js, $files);

    $info = wraith_aggregate_js_info($files);
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
        if (file_exists($file)) {
          if($filetime < filemtime($file)){
            $compile = TRUE;
          }
        }
      }
    }

    $js[$path] = $file = array(
      'type' => 'file',
      'group' => JS_LIBRARY,
      'every_page' => TRUE,
      'weight' => -1000,
      'scope' => 'header',
      'cache' => TRUE,
      'defer' => FALSE,
      'preprocess' => FALSE,
      'version' => $filetime,
      'data' => $path
    );

    if($compile){
      wraith_aggregate($files, $path, $directory);
      cache_set($cid, time(), 'cache_page', CACHE_TEMPORARY);
    }

    $files = $js;
  }else{
    $files['settings'] = $js['settings'];
    $js = $files;
  }
}

/**
 * Get global sass file information
 */
function wraith_aggregate_js_info(&$js = array()){
  global $theme_key;
  $info = &drupal_static(__FUNCTION__);
  if (!isset($info) && is_array($js) && !empty($js)) {
    $cid = implode('', array_keys($js));
    $info['cid'] = $theme_key.'-js-'.substr(drupal_hash_base64($cid), 0, 25);
    $files_directory = variable_get('file_' . file_default_scheme() . '_path', conf_path() . '/files') . '/wraith/' . $theme_key . '/js';
    $info['directory'] = theme_get_setting('wraith_compiler_destination') ? theme_get_setting('wraith_compiler_destination') . '/' . $theme_key . '/js' : $files_directory;
    $info['path'] = $info['directory'] . '/' . $theme_key . '.js';
    $info['filetime'] = file_exists($info['path']) ? filemtime($info['path']) : '';
  }
  return $info;
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
function wraith_aggregate($files, $filepath, $directory) {

  // Put $data in a file that Compass can compile.
  if (!file_prepare_directory($directory, FILE_CREATE_DIRECTORY)) {
    drupal_set_message(t('The directory !directory could not be created or is not writable. Please consult your !watchdog for a detailed error description.', array('!directory' => $directory, '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');
  };

  // Let assetic compile it
  require_once drupal_get_path('theme','wraith').'/includes/packer/class.JavaScriptPacker.php';

  $collection = array();
  foreach($files as $path){
    $collection[] = new FileAsset(DRUPAL_ROOT . '/' . $path);
  }

  $asset = new AssetCollection($collection, array(
    // Packer has trouble with missing delimiters is JS so for now we can't use it
    //$packer = new PackerFilter(),
  ));

  try {
    //$asset->load();
    $data = $asset->dump();

    // Compress file
    $data = wraith_uglifyjs($data);

    // If no exceptions were thrown, try saving the result to a JS file.
    //$data = gzencode($data, 9, FORCE_GZIP);
    if (!file_unmanaged_save_data($data, $filepath, FILE_EXISTS_REPLACE)) {
      drupal_set_message(t('The file <code>!file</code> could not be created. Please consult your !watchdog for a detailed error description.', array('!file' => $filepath, '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');
    }
  }
  catch (Exception $e) {
    // If compilation failed.
    drupal_set_message(t('Could not compile the file <code>!file</code>. Please consult your !watchdog for a detailed error description.', array('!file' => basename($filepath), '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');
    watchdog('wraith', '<pre>' . $e->getMessage() . '</pre>', NULL, WATCHDOG_DEBUG, NULL);
  }
}

function wraith_uglifyjs($data){
  try {
    $uglify_service = variable_get('wraith_uglify_endpoint', 'http://uglify.me') . '/uglify';
    $data = drupal_http_request(
      $uglify_service,
      array(
        'method' => 'POST',
        'data' => $data,
      )
    );
    if(!empty($data->data)){
      return $data->data;
    }
  }
  catch (Exception $e) {
    watchdog('wraith', 'Count not use uglifyjs to... ummmm... uglify the js.', NULL, WATCHDOG_DEBUG, NULL);
  }
  return $data;
}
