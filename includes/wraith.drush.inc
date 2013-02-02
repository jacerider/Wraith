<?php
/**
 * @file
 * This drush script creates a sub-theme for wraith.
 * Usage: drush wraith "Theme Name"
 *
 */

/**
 * Implementation of hook_drush_command().
 */
function wraith_drush_command() {
  $items = array();

  $items['wraith-new-subtheme'] = array(
    'description' => 'Set up a wraith starter kit.',
    'aliases' => array('wraith'),
    'arguments' => array(
      'name' => 'A name for your theme.',
      'machine_name' => '(optional)[a-z, 0-9] A machine-readable name for your theme.',
    ),
    'examples' => array(
      'example[1]' => 'drush wraith "My theme name"',
    ),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function wraith_drush_help($section) {
  switch ($section) {
    case 'drush:wraith-new-subtheme':
      return dt("Set up a wraith starter kit.");
  }
}


/**
 * Command: Set up a wraith starter kit.
 */
function drush_wraith_new_subtheme($name = NULL, $machine_name = NULL) {
  if (empty($name)) {
    drush_print(dt("\nWe can't create a subtheme with no name. \nPlease provide a name for the subtheme.\nEx. drush sns [name]\n"));
    return;
  }

  $machine_name = !empty($machine_name) ? preg_replace('/[^a-z0-9_]+/', '', strtolower($machine_name)) : preg_replace('/[^a-z0-9_]+/', '', strtolower($name));

  if (drupal_get_path('theme', 'wraith') == '') {
    drush_print(dt("\nWe could not find the path to wraith. \nPlease clear your cache and try again.\n"));
    return;
  }

  // Find Drupal install and wraith paths 
  $root_path = drush_locate_root();
  $wraith_relative = drupal_get_path('theme', 'wraith');

  // Construct relative path for new subtheme config
  $_wraith_relative = explode('/', $wraith_relative);
  array_pop($_wraith_relative);
  $themes_relative = implode('/', $_wraith_relative);
  $subtheme_relative = $themes_relative . '/' . $machine_name;

  // Construct absolute paths for copying
  $wraith_path = $root_path  . '/' . $wraith_relative; 
  $subtheme_path = $root_path . '/' . $subtheme_relative;

  // Make a fresh copy of the original starter kit.
  $s = drush_copy_dir("$wraith_path/SUB/", $subtheme_path);
  if (empty($s)) {
    return;
  }

  // Rename the info file and fill in the theme name.
  drush_op('rename', "$subtheme_path/SUB.info", "$subtheme_path/$machine_name.info");
  drush_op('rename', "$subtheme_path/assets/sass/SUB.scss", "$subtheme_path/assets/sass/$machine_name.scss");
  drush_op('rename', "$subtheme_path/assets/scripts/SUB.js", "$subtheme_path/assets/scripts/$machine_name.js");
  drush_op('rename', "$subtheme_path/assets/images/SUB.png", "$subtheme_path/assets/images/$machine_name.png");
  drush_op('wraith_file_str_replace', "$subtheme_path/$machine_name.info", ', <strong>you shouldn\'t enable this starterkit</strong>. instead, use <em>drush wraith</em> or manually copy it to create a new sub-theme, check out the <a href="http://www.linnovate.net/blog/wraith-smart-drupal-theming">docs</a> or the README.txt for more info', "");
  drush_op('wraith_file_str_replace', "$subtheme_path/$machine_name.info", 'sites/all/themes/SUB', "$subtheme_relative");
  drush_op('wraith_file_str_replace', "$subtheme_path/$machine_name.info", 'SUB', "$machine_name");
  drush_op('wraith_file_str_replace', "$subtheme_path/$machine_name.info", 'Starterkit', "$name");

  // Notify user of the newly created theme.
  drush_print(dt("\n!name sub-theme was created in !path. \n",
    array(
      '!name' => $name,
      '!path' => $subtheme_path,
    )
  ));

  drush_invoke_process("@self", "pm-enable", array($machine_name));

  if (drush_confirm(dt("\nWould you like to set !name as your default theme?", array('!name' => $name)))) {
    drush_op('variable_set', 'theme_default', $machine_name);
    drush_print(dt("!name was set as your default theme. \n", array('!name' => $name)));
  }

}


/**
 * Internal helper: Replace strings in a file.
 */
function wraith_file_str_replace($file_path, $find, $replace) {
  $file_contents = file_get_contents($file_path);
  $file_contents = str_replace($find, $replace, $file_contents);
  file_put_contents($file_path, $file_contents);
}
