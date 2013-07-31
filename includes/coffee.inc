<?php

// Composer autoload
require_once drupal_get_path('theme', 'wraith') . '/includes/assetic/vendor/autoload.php';

// Manage filters
use Assetic\Asset\FileAsset;
use Assetic\Filter\CompassFilter;
use Assetic\Filter\CoffeeScriptFilter;

/**
 * Aggregate Coffee files into JS
 *
 * @author JaceRider
 */
function wraith_aggregate_coffee(&$js){
  global $theme_key;

  $files = wraith_coffee_files($js);
  uasort($files, 'drupal_sort_weight');

  // We can bail out here if there are no Coffee files anyways.
  if (empty($files)) {
    return $js;
  }

  foreach($files as $filename => $file){

    $info = pathinfo($filename);
    $files_directory = variable_get('file_' . file_default_scheme() . '_path', conf_path() . '/files') . '/wraith/' . $theme_key . '/js';
    $dest_folder = theme_get_setting('wraith_coffee_compiler_destination') ? theme_get_setting('wraith_coffee_compiler_destination') . '/' . $theme_key . '/js' : $files_directory;
    $dest_file = $dest_folder.'/'.$info['filename'];

    // Initial checks to see if we should compile
    // - Does the compiled file exist?
    $compile = !file_exists($dest_file) ? TRUE : FALSE;
    // - If file has been updated
    if(!$compile && filemtime($dest_file) < filemtime($filename)){
      $compile = TRUE;
    }

    if($compile){
      wraith_coffee_compile($file, $dest_file, $dest_folder);
    }

    // Remove coffee file in $js with new file
    $js[$filename]['data'] = $dest_file;
  }
}

function wraith_coffee_compile($file, $filepath, $directory){

  // Create folder if needed
  if (!file_prepare_directory($directory, FILE_CREATE_DIRECTORY)) {
    drupal_set_message(t('The directory !directory could not be created or is not writable. Please consult your !watchdog for a detailed error description.', array('!directory' => $directory, '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');
  };

  $coffeeBin = theme_get_setting('wraith_coffee_coffeebin') ? theme_get_setting('wraith_coffee_coffeebin') : '/usr/bin/coffee';
  $nodeBin = theme_get_setting('wraith_coffee_nodebin') ? theme_get_setting('wraith_coffee_nodebin') : '/usr/bin/node';
  // Let assetic compile it
  $asset = new FileAsset($file['data'],
    array(
      $coffee = new CoffeeScriptFilter($coffeeBin, $nodeBin),
    )
  );

  try {
    $data = $asset->dump();
    // If no exceptions were thrown, try saving the result to a CSS file.
    if (!file_unmanaged_save_data($data, $filepath, FILE_EXISTS_REPLACE)) {
      drupal_set_message(t('The file <code>!file</code> could not be created. Please consult your !watchdog for a detailed error description.', array('!file' => $css_filepath, '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');
    };

  }
  catch (Exception $e) {
    // If compilation failed.
    drupal_set_message(t('Could not compile the file <code>!file</code>. Please consult your !watchdog for a detailed error description.', array('!file' => basename($file['data']), '!watchdog' => l('log messages', 'admin/reports/dblog'))), 'error');
    watchdog('wraith', '<pre>' . $e->getMessage() . '</pre>', NULL, WATCHDOG_DEBUG, NULL);
  }
}

/**
 * Picks all Coffee files from an array of js files.
 *
 * @param $items
 *   An array of js files.
 *
 * @return
 *   The extracted files as an array.
 */
function wraith_coffee_files($items) {
  $files = &drupal_static(__FUNCTION__);
  if (!isset($files)){
    $files = array();
    foreach ($items as $key => $file) {
      if ($file['type'] == 'file') {
        $extension = drupal_substr($file['data'], -7);
        if (in_array($extension, array('.coffee'))) {
          $file['syntax'] = 'coffee';
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
