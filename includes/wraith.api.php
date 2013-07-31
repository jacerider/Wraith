<?php

/**
 * @file
 * Hooks provided by the Wraith theme.
 */

/**
 * Allows altering the data (string) of SASS or SCSS file
 * just before it's processed by Compass.
 * Only stylesheets with 'wraith_alter' set to TRUE will go through this hook.
 * Example usage:
 * 
 *   drupal_add_css(
 *     drupal_get_path('theme', 'wraith') . '/sass/extra/web-fonts.scss',
 *     array(
 *       'theme' => 'wraith',
 *       'wraith_alter' => true
 *     )
 *   );
 *
 * @param &$data
 *   The SASS or SCSS file content of $file (string) 
 *   that is going to be processed by the Compass.
 */
function hook_wraith_alter(&$data) {
  // Replaces '[oranges]' with 'apples' and '[apples]' with 'oranges'.
  $variables = array(
    '[oranges]' => 'apples',
    '[apples]' => 'oranges',
  );

  $data = str_replace(array_keys($variables), $variables, $data);
}
