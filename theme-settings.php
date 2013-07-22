<?php

/**
 * @file
 * Theme settings for the Wraith
 */
function wraith_form_system_theme_settings_alter(&$form, &$form_state) {

  drupal_add_css(drupal_get_path('theme', 'wraith') .'/assets/styles/theme-settings.css');
  drupal_add_js(drupal_get_path('theme', 'wraith') .'/assets/scripts/themeSettings.js');

  $select_toggle = '<br>' .
  l(t('select all'), '#', array('attributes' => array('class' => 'select-all'))) . ' | ' .
  l(t('select none'), '#', array('attributes' => array('class' => 'select-none')));

  $form['wraith_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => -10,
    '#prefix' => '<h3>' . t('Theme configuration') . '</h3>',
  );

  /**
   * SASS/Compass Settings
   */
  $form['wraith_settings']['wraith_compass'] = array(
    '#type' => 'fieldset',
    '#title' => t('Compass Settings'),
  );
  $form['wraith_settings']['wraith_compass']['wraith_compass_info'] = array(
    '#type' => 'markup',
    '#markup' => t('<h4>This theme now makes use of the Sonar module to handle SCSS/Compass compiling.</h4>'),
  );
  if(module_exists('sonar'))
  {
    $form['wraith_settings']['wraith_compass']['wraith_compass_status'] = array(
      '#type' => 'markup',
      '#markup' => '<div class="messages status">'.t('The Sonar module is enabled. You can access its settings !url.', array('!url' => l('here', 'admin/config/system/sonar'))).'</div>',
    );
  }else{
    $form['wraith_settings']['wraith_compass']['wraith_compass_status'] = array(
      '#type' => 'markup',
      '#markup' => '<div class="messages error">'.t('Please download the Sonar module (!url) and enable it to utilize SCSS in this theme.', array('!url' => l('https://github.com/JaceRider/Sonar', 'https://github.com/JaceRider/Sonar'))).'</div>',
    );
  }

  /**
   * Bootstrap settings
   */
  $form['wraith_settings']['wraith_bootstrap'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bootstrap settings'),
  );
  $form['wraith_settings']['wraith_bootstrap']['wraith_bootstrap'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Bootstrap'),
    '#attributes' => array(
      'class' => array('enable-extension'),
    ),
    '#description' => t('Use Twitter Bootstrap with this theme.'),
    '#default_value' => theme_get_setting('wraith_bootstrap'),
  );
  require_once drupal_get_path('theme', 'wraith') . '/includes/assets.inc';
  $form['wraith_settings']['wraith_bootstrap']['wraith_bootstrap_components'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bootstrap Assets'),
  );
  $form['wraith_settings']['wraith_bootstrap']['wraith_bootstrap_components']['css'] = array(
    '#type' => 'fieldset',
    '#title' => t('SCSS Components'),
    '#description' => t('Enable/Disable Bootstrap CSS Components') . $select_toggle,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['wraith_settings']['wraith_bootstrap']['wraith_bootstrap_components']['css']['wraith_bootstrap_scss'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Files'),
    '#options' => wraith_get_bootstrap_scss(),
    '#default_value' => theme_get_setting('wraith_bootstrap_scss') ? theme_get_setting('wraith_bootstrap_scss') : array(),
  );
  $form['wraith_settings']['wraith_bootstrap']['wraith_bootstrap_components']['js'] = array(
    '#type' => 'fieldset',
    '#title' => t('Javascript Components'),
    '#description' => t('Enable/Disable Bootstrap Javascript Components') . $select_toggle,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['wraith_settings']['wraith_bootstrap']['wraith_bootstrap_components']['js']['wraith_bootstrap_js'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Files'),
    '#options' => wraith_get_bootstrap_js(),
    '#default_value' => theme_get_setting('wraith_bootstrap_js') ? theme_get_setting('wraith_bootstrap_js') : array(),
  );
  $form['wraith_settings']['wraith_bootstrap']['wraith_bootstrap_overrides'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Bootstrap theme spectacular'),
    '#description' => t('Theme overrides that make Drupal more Bootstrappy.'),
    '#default_value' => theme_get_setting('wraith_bootstrap_overrides'),
    '#prefix' => '<br />',
  );

  /**
   * Bootstrap settings
   */
  $form['wraith_settings']['wraith_font'] = array(
    '#type' => 'fieldset',
    '#title' => t('Font settings'),
  );

  /**
   * CSS settings
   */
  $form['wraith_settings']['wraith_css'] = array(
    '#type' => 'fieldset',
    '#title' => t('CSS settings'),
  );
  // Disable CSS
  require_once drupal_get_path('theme', 'wraith') . '/includes/assets.inc';
  $form['wraith_settings']['wraith_css']['wraith_enablecss'] = array(
    '#type' => 'fieldset',
    '#title' => t('Enable Specific CSS files'),
  );
  $form['wraith_settings']['wraith_css']['wraith_enablecss']['wraith_enable_css'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable this extension. '),
    '#default_value' => theme_get_setting('wraith_enable_css'),
    '#description' => t('Removes all Core and Module provided CSS files.'),
    '#attributes' => array(
      'class' => array('enable-extension'),
      'data-disable' => '#edit-wraith-disablecss',
    ),
  );
  $form['wraith_settings']['wraith_css']['wraith_enablecss']['modules'] = array(
    '#type' => 'fieldset',
    '#title' => t('Per module'),
    '#description' => t('Enable all CSS files from selected modules.') . $select_toggle,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['wraith_settings']['wraith_css']['wraith_enablecss']['modules']['wraith_enable_css_modules'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Modules'),
    '#options' => wraith_get_modules_list(),
    '#default_value' => theme_get_setting('wraith_enable_css_modules') ? theme_get_setting('wraith_enable_css_modules') : array(),
  );
  $form['wraith_settings']['wraith_css']['wraith_enablecss']['files'] = array(
    '#type' => 'fieldset',
    '#title' => t('Specific CSS files'),
    '#description' => t('Enable specific CSS files from core and contrib modules.') . $select_toggle,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['wraith_settings']['wraith_css']['wraith_enablecss']['files']['wraith_enable_css_files'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Files'),
    '#options' => wraith_get_assets_list(),
    '#default_value' => theme_get_setting('wraith_enable_css_files') ? theme_get_setting('wraith_enable_css_files') : array(),
  );
  $form['wraith_settings']['wraith_css']['wraith_disablecss'] = array(
    '#type' => 'fieldset',
    '#title' => t('Disable Specific CSS files'),
  );
  $form['wraith_settings']['wraith_css']['wraith_disablecss']['wraith_disable_css'] = array(
    '#type' => 'checkbox',
    '#title' => t("Enable this extension."),
    '#attributes' => array(
      'class' => array('enable-extension'),
      'data-disable' => '#edit-wraith-enablecss',
    ),
    '#description' => t('Disable all CSS files included by core and contrib modules or choose specific CSS files to disable.'),
    '#default_value' => theme_get_setting('wraith_disable_css'),
  );
  $form['wraith_settings']['wraith_css']['wraith_disablecss']['modules'] = array(
    '#type' => 'fieldset',
    '#title' => t('Per module'),
    '#description' => t('Disable all CSS files from selected modules.') . $select_toggle,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['wraith_settings']['wraith_css']['wraith_disablecss']['modules']['wraith_disable_css_modules'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Modules'),
    '#options' => wraith_get_modules_list(),
    '#default_value' => theme_get_setting('wraith_disable_css_modules') ? theme_get_setting('wraith_disable_css_modules') : array(),
  );
  $form['wraith_settings']['wraith_css']['wraith_disablecss']['files'] = array(
    '#type' => 'fieldset',
    '#title' => t('Specific CSS files'),
    '#description' => t('Disable specific CSS files from core and contrib modules.') . $select_toggle,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['wraith_settings']['wraith_css']['wraith_disablecss']['files']['wraith_disable_css_files'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Files'),
    '#options' => wraith_get_assets_list(),
    '#default_value' => theme_get_setting('wraith_disable_css_files') ? theme_get_setting('wraith_disable_css_files') : array(),
  );

  /**
   * Coffee Settings
   */
  $form['wraith_settings']['wraith_coffee'] = array(
    '#type' => 'fieldset',
    '#title' => t('Coffee Settings'),
  );
  $form['wraith_settings']['wraith_coffee']['wraith_coffee'] = array(
    '#type' => 'checkbox',
    '#title' => t('Compile Coffee to JS'),
    '#attributes' => array(
      'class' => array('enable-extension'),
    ),
    '#description' => t('Write your code as coffee in files title *.js.coffee'),
    '#default_value' => theme_get_setting('wraith_coffee'),
  );
  $form['wraith_settings']['wraith_coffee']['wraith_coffee_coffeebin'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to Coffee Bin'),
    '#description' => t('Default path is %default.', array('%default' => '/usr/bin/coffee')),
    '#attributes' => array(
      'placeholder' => t('/usr/bin/coffee'),
    ),
    '#default_value' => theme_get_setting('wraith_coffee_coffeebin'),
  );
  $form['wraith_settings']['wraith_coffee']['wraith_coffee_nodebin'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to NodeJS Bin'),
    '#description' => t('Default path is %default.', array('%default' => '/usr/bin/node')),
    '#attributes' => array(
      'placeholder' => t('/usr/bin/node'),
    ),
    '#default_value' => theme_get_setting('wraith_coffee_nodebin'),
  );
  $files_directory = variable_get('file_' . file_default_scheme() . '_path', conf_path() . '/files') . '/wraith';
  $form['wraith_settings']['wraith_coffee']['wraith_coffee_compiled_path'] = array(
    '#type' => 'fieldset',
    '#title' => t('Compiled files path'),
  );
  $form['wraith_settings']['wraith_coffee']['wraith_coffee_compiled_path']['description'] = array(
    '#markup' => '<div class="description">' . t('Set the path to where you would like compiled files to be stored. defaults to <code>!files</code>', array('!files' => $files_directory)) . '<br />' .
    t('Compiled files will be stored in a sub-directory with the theme name so entering the path to your themes directory here will place the copiled files in a <code>/js/</code> directory under each theme\'s directory.') . '</div>',
  );
  $form['wraith_settings']['wraith_coffee']['wraith_coffee_compiled_path']['wraith_coffee_compiler_destination'] = array(
    '#type' => 'textfield',
    '#attributes' => array(
      'placeholder' => t('e.g.') . ' sites/all/themes',
    ),
    '#default_value' => theme_get_setting('wraith_coffee_compiler_destination'),
  );

  /**
   * JavaScript Settings
   */
  $form['wraith_settings']['wraith_js'] = array(
    '#type' => 'fieldset',
    '#title' => t('JavaScript Settings'),
  );
  $form['wraith_settings']['wraith_js']['wraith_jquery'] = array(
    '#type' => 'fieldset',
    '#title' => t('jQuery'),
  );
  $form['wraith_settings']['wraith_js']['wraith_jquery']['jquery_update_jquery_version'] = array(
    '#type' => 'select',
    '#title' => t('jQuery Version'),
    '#options' => array(
      '1.5' => '1.5',
      '1.7' => '1.7',
      '1.8' => '1.8',
    ),
    '#default_value' => theme_get_setting('jquery_update_jquery_version') ? theme_get_setting('jquery_update_jquery_version') : '1.7',
    '#description' => t('Select which jQuery version branch to use.'),
  );
  $form['wraith_settings']['wraith_js']['wraith_jquery']['jquery_update_compression_type'] = array(
    '#type' => 'radios',
    '#title' => t('jQuery compression level'),
    '#options' => array(
      'min' => t('Production (minified)'),
      'none' => t('Development (uncompressed)'),
    ),
    '#default_value' => theme_get_setting('jquery_update_compression_type') ? theme_get_setting('jquery_update_compression_type') : 'min',
  );
  $form['wraith_settings']['wraith_js']['wraith_jquery']['jquery_update_jquery_cdn'] = array(
    '#type' => 'select',
    '#title' => t('jQuery and jQuery UI CDN'),
    '#options' => array(
      'none' => t('None'),
      'google' => t('Google'),
      'microsoft' => t('Microsoft'),
      'jquery' => t('jQuery'),
    ),
    '#default_value' => theme_get_setting('jquery_update_jquery_cdn') ? theme_get_setting('jquery_update_jquery_cdn') : 'none',
    '#description' => t('Use jQuery and jQuery UI from a CDN. If the CDN is not available the local version of jQuery and jQuery UI will be used.'),
  );
  $form['wraith_settings']['wraith_js']['wraith_js_footer_wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Move JavaScript to footer'),
  );
  $form['wraith_settings']['wraith_js']['wraith_js_footer_wrapper']['wraith_js_footer'] = array(
    '#type' => 'checkbox',
    '#title' => t('Move all scripts to the footer.'),
    '#description' => t('Yahoo! Exceptional Performance team recommends <a href="http://developer.yahoo.com/performance/rules.html#js_bottom">placing scripts at the bottom of your page</a> because of the way browsers download components.') . '<br>' .
      t('This will move all your scripts to the bottom of your page. You can still force a script to go in the <code>head</code> by setting <code>"force_header" => TRUE</code> in your !drupal_add_js options array.', array('!drupal_add_js' => l('drupal_add_js', 'http://api.drupal.org/api/drupal/includes%21common.inc/function/drupal_add_js', array('attributes' => array('target'=>'_blank'))))),
    '#default_value' => theme_get_setting('wraith_js_footer'),
  );
  // Aggregate JS
  $form['wraith_settings']['wraith_js']['wraith_aggregatejs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Aggregate JS files'),
  );
  $form['wraith_settings']['wraith_js']['wraith_aggregatejs']['wraith_aggregate_js'] = array(
    '#type' => 'checkbox',
    '#title' => t("Enable this extension."),
    '#description' => t('Select files that will be agreegate into a global file.'),
    '#default_value' => theme_get_setting('wraith_aggregate_js'),
  );
  $form['wraith_settings']['wraith_js']['wraith_aggregatejs']['files'] = array(
    '#type' => 'fieldset',
    '#title' => t('Specific JS files'),
    '#description' => t('Aggregate specific JS files from core and contrib modules.') . $select_toggle,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $js = wraith_get_assets_list('js');
  $saved = theme_get_setting('wraith_aggregate_js_files');
  $saved = empty($saved) ? array() : $saved;
  $new = array();
  foreach($saved as $key => $value){
    if($value) $new[$key] = $value;
  }
  $saved = array_merge($new,$saved);
  $options = array_merge($saved,$js);
  $form['wraith_settings']['wraith_js']['wraith_aggregatejs']['files']['wraith_aggregate_js_files'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Aggregate specific JS files.'),
    '#options' => $options,
    '#default_value' => theme_get_setting('wraith_aggregate_js_files') ? theme_get_setting('wraith_aggregate_js_files') : array(),
    '#after_build' => array('wraith_aggregate_js_files_after'),
  );
  // Disable JS
  $form['wraith_settings']['wraith_js']['wraith_disablejs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Disable JS files'),
  );
  $form['wraith_settings']['wraith_js']['wraith_disablejs']['wraith_disable_js'] = array(
    '#type' => 'checkbox',
    '#title' => t("Enable this extension."),
    '#attributes' => array(
      'class' => array('enable-extension'),
    ),
    '#description' => t('Disable all JS files included by core and contrib modules or choose specific JS files to disable.'),
    '#default_value' => theme_get_setting('wraith_disable_js'),
  );
  $form['wraith_settings']['wraith_js']['wraith_disablejs']['modules'] = array(
    '#type' => 'fieldset',
    '#title' => t('Per module'),
    '#description' => t('Disable all JS files from selected modules.') . $select_toggle,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['wraith_settings']['wraith_js']['wraith_disablejs']['modules']['wraith_disable_js_modules'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Modules'),
    '#options' => wraith_get_modules_list(),
    '#default_value' => theme_get_setting('wraith_disable_js_modules') ? theme_get_setting('wraith_disable_js_modules') : array(),
  );
  $form['wraith_settings']['wraith_js']['wraith_disablejs']['files'] = array(
    '#type' => 'fieldset',
    '#title' => t('Specific JS files'),
    '#description' => t('Disable specific JS files from core and contrib modules.') . $select_toggle,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['wraith_settings']['wraith_js']['wraith_disablejs']['files']['wraith_disable_js_files'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Disable specific JS files.'),
    '#options' => wraith_get_assets_list('js'),
    '#default_value' => theme_get_setting('wraith_disable_js_files') ? theme_get_setting('wraith_disable_js_files') : array(),
  );
  $form['wraith_settings']['wraith_js']['wraith_disablejs']['jquery'] = array(
    '#type' => 'fieldset',
    '#title' => t('Core jQuery & jQuery UI'),
    '#description' => t('Disable specific jQuery & jQuery UI files from core.') . $select_toggle,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['wraith_settings']['wraith_js']['wraith_disablejs']['jquery']['wraith_disable_jquery_files'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Disable core jQuery & jQuery UI files.'),
    '#options' => wraith_get_assets_list('js', 'jquery'),
    '#default_value' => theme_get_setting('wraith_disable_jquery_files') ? theme_get_setting('wraith_disable_jquery_files') : array(),
  );
  $form['wraith_settings']['wraith_js']['wraith_disablejs']['wraith_js'] = array(
    '#type' => 'fieldset',
    '#title' => t('wraith\'s JS'),
  );
  $form['wraith_settings']['wraith_js']['wraith_disablejs']['wraith_js']['wraith_disable_wraith_js'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable all JS from Wraith.'),
    '#description' => t('<strong>Note:</strong> this will break wraith\'s scripts like the file-watcher and mobile menus if they are enabled, be warned.'),
    '#default_value' => theme_get_setting('wraith_disable_wraith_js'),
  );

  /**
   * HTML5 IE support
   */
  $form['wraith_settings']['wraith_tools'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tools & Utilities'),
  );
  $form['wraith_settings']['wraith_tools']['wraith_modernizr'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Modernizr'),
    '#description' => t('<a href="!modernizr">Modernizr</a> is a JavaScript library that detects HTML5 and CSS3 features in the user’s browser. This <strong>includes HTML5 Shiv</strong> so make sure not to enable it in the settings below.', array('!modernizr' => 'http://modernizr.com/')),
    '#default_value' => theme_get_setting('wraith_modernizr'),
  );
  $form['wraith_settings']['wraith_tools']['wraith_ie'] = array(
    '#type' => 'fieldset',
    '#title' => t('Internet Explorer'),
    '#description' => t('Settings specific to IE'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['wraith_settings']['wraith_tools']['wraith_ie']['wraith_force_ie'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force latest IE rendering engine (even in intranet) & Chrome Frame'),
    '#default_value' => theme_get_setting('wraith_force_ie'),
  );
  $form['wraith_settings']['wraith_tools']['wraith_ie']['wraith_shiv'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable HTML5 elements in IE'),
    '#description' => t('Makes IE understand HTML5 elements via <a href="!shivlink">HTML5 shiv</a>. disable if you use a different method.', array('!shivlink' => 'http://code.google.com/p/html5shiv/')),
    '#default_value' => theme_get_setting('wraith_toolsshiv'),
  );
  $form['wraith_settings']['wraith_tools']['wraith_ie']['wraith_ie_comments'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add IE specific classes'),
    '#description' => t('This will add conditional classes to the html tag for IE specific styling. see this <a href="!post">post</a> for more info.', array('!post' => 'http://paulirish.com/2008/conditional-stylesheets-vs-css-hacks-answer-neither/')),
    '#default_value' => theme_get_setting('wraith_ie_comments'),
  );
  $form['wraith_settings']['wraith_tools']['wraith_ie']['wraith_prompt_cf'] = array(
    '#type' => 'select',
    '#title' => t('Prompt IE users to install Chrome Frame'),
    '#default_value' => theme_get_setting('wraith_prompt_cf'),
    '#options' => drupal_map_assoc(array(
       'Disabled',
       'IE 6',
       'IE 7',
       'IE 8',
       'IE 9',
    )),
      '#description' => t('Set the latest IE version you would like the prompt box to show on or disable if you want to support old IEs.'),
  );

  /**
   * Development Settings
   */
  $form['wraith_settings']['wraith_development'] = array(
    '#type' => 'fieldset',
    '#title' => t('Development'),
  );
  $form['wraith_settings']['wraith_development']['wraith_watch'] = array(
    '#type' => 'fieldset',
    '#title' => t('File Watcher'),
  );
  $form['wraith_settings']['wraith_development']['wraith_watch']['wraith_watcher'] = array(
    '#type' => 'checkbox',
    '#title' => t('Watch for file changes and automatically refresh the browser.'),
    '#attributes' => array(
      'class' => array('enable-extension'),
    ),
    '#description' => t('With this feature on, you may enter a list of URLs for files to be watched, whenever a file is changed, your browser will automagically refresh itself.<br><strong>Turn this off when not actively developing.</strong>'),
    '#default_value' => theme_get_setting('wraith_watcher'),
  );
  $form['wraith_settings']['wraith_development']['wraith_watch']['wraith_watch_sass'] = array(
    '#type' => 'checkbox',
    '#title' => t('Watch Theme Sass'),
    '#description' => t('Enable watching of all theme SASS files automatically.'),
    '#default_value' => theme_get_setting('wraith_watch_sass'),
  );
  $form['wraith_settings']['wraith_development']['wraith_watch']['wraith_watch_file'] = array(
    '#type' => 'textarea',
    '#title' => t('Files to watch'),
    '#description' => t('Enter the internal path of the files to be watched. one file per line. no leading slash.<br> e.g. sites/all/themes/wraith/stylesheets/wraith.scss<br>Lines starting with a semicolon (;) will be ignored.<br><strong>Keep this list short !</strong> Watch only the files you currently work on.'),
    '#rows' => 3,
    '#default_value' => theme_get_setting('wraith_watch_file'),
  );
  $form['wraith_settings']['wraith_development']['wraith_watch']['wraith_instant_watcher'] = array(
    '#type' => 'checkbox',
    '#title' => t('Update styles without refreshing.'),
    '#description' => t('<strong>Experimental</strong> - this will instantly update your browser when a watched file is updated without refreshing the browser. note: this will work with stylesheets only (CSS/SASS/SCSS).'),
    '#default_value' => theme_get_setting('wraith_instant_watcher'),
  );

  /**
   * General Settings
   */
  $form['wraith_settings']['wraith_general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General'),
  );

  $form['wraith_settings']['wraith_general']['theme_settings'] = $form['theme_settings'];
  $form['wraith_settings']['wraith_general']['logo'] = $form['logo'];
  $form['wraith_settings']['wraith_general']['favicon'] = $form['favicon'];
  unset($form['theme_settings']);
  unset($form['logo']);
  unset($form['favicon']);

  $form['wraith_settings']['wraith_general']['wraith_breadcrumb'] = array(
    '#type' => 'fieldset',
    '#title' => t('Breadcrumbs'),
  );
  $form['wraith_settings']['wraith_general']['wraith_breadcrumb']['wraith_breadcrumb_hideonlyfront'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide the breadcrumb if the breadcrumb only contains the link to the front page.'),
    '#default_value' => theme_get_setting('wraith_breadcrumb_hideonlyfront'),
  );
  $form['wraith_settings']['wraith_general']['wraith_breadcrumb']['wraith_breadcrumb_showtitle'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show page title on breadcrumb.'),
    '#default_value' => theme_get_setting('wraith_breadcrumb_showtitle'),
  );
  $form['wraith_settings']['wraith_general']['wraith_breadcrumb']['wraith_breadcrumb_separator'] = array(
    '#type' => 'textfield',
    '#title' => t('Breadcrumb separator'),
    '#default_value' => theme_get_setting('wraith_breadcrumb_separator'),
  );

  $form['wraith_settings']['wraith_general']['wraith_rss'] = array(
    '#type' => 'fieldset',
    '#title' => t('RSS'),
  );
  $form['wraith_settings']['wraith_general']['wraith_rss']['wraith_feed_icons'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display Feed Icons'),
    '#default_value' => theme_get_setting('wraith_feed_icons'),
  );

  $form['#validate'][] = 'wraith_form_system_theme_settings_alter_validate';

}

/**
 * Clean up the form values before they are saved
 *
 * @author JaceRider
 */
function wraith_form_system_theme_settings_alter_validate($form, &$form_state){
  $values = &$form_state['values'];
  $values = _array_remove_null($values);

  // If using Font Awesome -- deselect Bootstrap sprites
  if(module_exists('fawesome') && !empty($values['wraith_bootstrap_scss']['_sprites'])){
    unset($values['wraith_bootstrap_scss']['_sprites']);
    drupal_set_message('Bootstrap sprites have been disabled to make way for Font Awesome to be awesome.', 'warning');
  }
}

function wraith_aggregate_js_files_after($element) {
  //dsm($element);

  drupal_add_tabledrag('wraith-aggregate-sort', 'order', 'sibling', 'weight'); // needed for table dragging

  $header = array(
    'markup' => '',
    'weight' => '',
  );

  $rows = array();
  foreach (element_children($element) as $key) {
    $row = array();

    $row['data'][] = drupal_render($element[$key]);
    $row['class'] = array('draggable');

    $weight = array(
      '#type' => 'textfield',
      '#attributes' => array('class'=>array('weight'))
    );
    $row['data'][] = drupal_render($weight);
    $row['class'] = array('draggable');
    // foreach ($header as $fieldname => $title) {
    //   $row['data'][] = drupal_render($element[$key]);
    //   $row['class'] = array('draggable'); // needed for table dragging
    // }
    $rows[] = $row;
  }

  $element['table'] = array(
    '#markup' =>theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('id' => 'wraith-aggregate-sort'), // needed for table dragging
    )
  ));

  return $element;
}

/**
 * Remove empty values from nested array
 *
 * @author JaceRider
 */
function _array_remove_null($array) {
  foreach ($array as $key => $value){
    if(in_array($key, array('logo_path','favicon_path','logo_upload','favicon_upload'))) continue;
    if(empty($value))
      unset($array[$key]);
    if(is_array($value))
      $array[$key] = _array_remove_null($value);
  }
  return $array;
}
