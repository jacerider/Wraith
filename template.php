<?php

$path_to_wraith = drupal_get_path('theme', 'wraith');
require_once $path_to_wraith . '/includes/assets.inc';     // CSS & JS alters
require_once $path_to_wraith . '/includes/preprocess.inc'; // Preprocess hooks
require_once $path_to_wraith . '/includes/theme.inc';      // Theme functions

/**
 * Render SASS files.
 */
if (theme_get_setting('wraith_sass')) {
  require_once $path_to_wraith . '/includes/sass.inc';     // Sass compiler
  require_once $path_to_wraith . '/includes/alter.inc';    // wraith tokens
  wraith_sass_compile();
}

/**
 * Bootstrap
 */
if (theme_get_setting('wraith_bootstrap_overrides')) {
  require_once $path_to_wraith . '/includes/bootstrap.inc';     // Bootstrap hooks
}
