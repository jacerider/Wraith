<?php

// Composer autoload
require_once drupal_get_path('theme', 'wraith') . '/includes/assetic/vendor/autoload.php';

// Manage filters
use Assetic\Asset\AssetCollection;
use Assetic\Asset\FileAsset;
use Assetic\Filter\PackerFilter;

function wraith_aggregate_js(&$js){
  $files = &drupal_static(__FUNCTION__);
  if (!isset($files)) {
    $files = array('header'=>array(),'footer'=>array(),'other'=>array());
    foreach ($js as $filepath => &$item) {
      if($item['type'] == 'file' && !empty($item['every_page']) && empty($item['wraith_aggregate'])){
        $scope = $item['scope'] == 'header' || $item['scope'] == 'footer' ? $item['scope'] : 'other';
        $files[$scope][$filepath] = $item;
        unset($js[$filepath]);
      }
    }

    // Keep render order
    $files = $files['header'] + $files['other'] + $files['footer'];
    uasort($files, 'drupal_sort_weight');

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
        if (file_exists($file['data'])) {
          if($filetime < filemtime($file['data'])){
            $compile = TRUE;
          }
        }
      }
    }

    // Loop through each file again and remove from $elements array
    // and read if we are compiling
    foreach ($files as $key => $file) {
      if (file_exists($file['data'])) {
        // We recompile this file if the file doesn't exist, if recompile equals true,
        // if cache was cleared or if the file have changed since it was last compiled.
        if ($compile){
          // Read the file.
          $data[] = $key;
        }
      }
    }

    $file['scope'] = 'header';
    $file['data'] = $path;
    $file['version'] = $filetime;
    $file['weight'] = $file['group'] = -1000;
    $js[$path] = $file;
    drupal_add_js($path, array('every_page' => TRUE, 'wraith_aggregate' => TRUE));

    if(!empty($data)){
      // Compile.
      dsm('Compiled');
      $file['directory'] = $directory;
      wraith_aggregate($data, $file);
      cache_set($cid, time(), 'cache_page', CACHE_TEMPORARY);
    }

    $files = $js;
  }else{
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
function wraith_aggregate($data, $file) {
  $js_filepath = $file['data'];
  $directory = $file['directory'];

  // Put $data in a file that Compass can compile.
  if (!file_prepare_directory($directory, FILE_CREATE_DIRECTORY)) {
    drupal_set_message(t('The directory !directory could not be created or is not writable. Please consult your !watchdog for a detailed error description.', array('!directory' => $directory, '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');
  };

  // Let assetic compile it
  require_once drupal_get_path('theme','wraith').'/includes/packer/class.JavaScriptPacker.php';
  // $asset = new FileAsset($js_filepath,
  //   array(
  //     $packer = new PackerFilter(),
  //   )
  // );

  $collection = array();
  foreach($data as $path){
    $collection[] = new FileAsset(DRUPAL_ROOT . '/' . $path);
  }

  $asset = new AssetCollection($collection, array(
    $packer = new PackerFilter(),
  ));

  try {

    $asset->load();
    $data = $asset->dump();
    // If no exceptions were thrown, try saving the result to a JS file.
    //$data = gzencode($data, 9, FORCE_GZIP);
    if (!file_unmanaged_save_data($data, $js_filepath, FILE_EXISTS_REPLACE)) {
      drupal_set_message(t('The file <code>!file</code> could not be created. Please consult your !watchdog for a detailed error description.', array('!file' => $js_filepath, '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');
    };
  }
  catch (Exception $e) {
    // If compilation failed.
    drupal_set_message(t('Could not compile the file <code>!file</code>. Please consult your !watchdog for a detailed error description.', array('!file' => basename($file['data']), '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');
    watchdog('wraith', '<pre>' . $e->getMessage() . '</pre>', NULL, WATCHDOG_DEBUG, NULL);
  }
}
