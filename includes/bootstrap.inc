<?php

/**
 * Implements template_preprocess_page().
 */
function wraith_bootstrap_preprocess_page(&$vars){

  // If we have both sidebars
  if(!empty($vars['page']['sidebar_first']) && !empty($vars['page']['sidebar_last'])){
    $vars['sidebar_class'] = 'span3';
    $vars['content_class'] = 'span6';
  }
  // If we only have one sidebar
  elseif(!empty($vars['page']['sidebar_first']) || !empty($vars['page']['sidebar_last'])){
    $vars['sidebar_class'] = 'span3';
    $vars['content_class'] = 'span9';
  // We we don't have any sidebars
  }else{
    $vars['content_class'] = 'span12';
  }

}

/**
 * Returns HTML for status and/or error messages, grouped by type.
 */
function wraith_status_messages($variables) {
  $display = $variables['display'];
  $output = '';

  $status_heading = array(
    'status' => t('Status message'),
    'error' => t('Error message'),
    'warning' => t('Warning message'),
  );

  // Map Drupal message types to their corresponding Bootstrap classes.
  // @see http://twitter.github.com/bootstrap/components.html#alerts
  $status_class = array(
    'status' => 'success',
    'error' => 'error',
    'warning' => 'info',
  );

  foreach (drupal_get_messages($display) as $type => $messages) {
    $class = (isset($status_class[$type])) ? ' alert-' . $status_class[$type] : '';
    $output .= "<div class=\"alert alert-block$class\">\n";
    $output .= "  <a class=\"close\" data-dismiss=\"alert\" href=\"#\">x</a>\n";

    if (!empty($status_heading[$type])) {
      $output .= '<h2 class="element-invisible">' . $status_heading[$type] . "</h2>\n";
    }

    if (count($messages) > 1) {
      $output .= " <ul>\n";
      foreach ($messages as $message) {
        $output .= '  <li>' . $message . "</li>\n";
      }
      $output .= " </ul>\n";
    }
    else {
      $output .= $messages[0];
    }

    $output .= "</div>\n";
  }
  return $output;
}

/**
* Preprocessor for theme('button').
*/
function wraith_preprocess_button(&$vars) {
  $vars['element']['#attributes']['class'][] = 'btn';

  if (isset($vars['element']['#value'])) {
    if(module_exists('wraith_core') && $class = wraith_button_class_attach($vars['element']['#value'])){
      $vars['element']['#attributes']['class'][] = $class;
    }

    if(module_exists('wraith_core') && $icon = wraith_icons_attach($vars['element']['#value'])){
      $vars['element']['#field_prefix'] = $icon;
    }
  }
}

/**
* Returns HTML for a button form element.
*/
function wraith_button($vars) {

  $element = $vars['element'];
  $element['#attributes']['type'] = 'submit';
  $label = check_plain($element['#value']);
  element_set_attributes($element, array('id', 'name', 'value'));

  $element['#attributes']['class'][] = 'form-' . $element['#button_type'];
  if (!empty($element['#attributes']['disabled'])) {
    $element['#attributes']['class'][] = 'form-button-disabled';
  }

  $field_prefix = isset($element['#field_prefix']) ? $element['#field_prefix'] : '';



  // Some elements do not work as buttons
  $skip = false;
  if(isset($element['#op']) && $element['#op'] == 'refresh_table') $skip = true;
  if(!empty($element['#no_button'])) $skip = true;
  if(!empty($element['#id']) && in_array($element['#id'], array('views-add-group'))) $skip = true;
  if($skip){
    $element['#attributes']['type'] = 'submit';
    return '<input' . drupal_attributes($element['#attributes']) . ' />';
  }

  return '<button' . drupal_attributes($element['#attributes']) . '>'. $field_prefix . $label .'</button>
'; // This line break adds inherent margin between multiple buttons
}

function wraith_preprocess_links(&$vars){
  if(isset($vars['attributes']['id'])){
    $id = str_replace('-','_',$vars['attributes']['id']);
    if(in_array($id, array('main_menu_links','secondary_menu_links'))){
      $vars['theme_hook_suggestions'][] = 'links__both';
    }
    $vars['theme_hook_suggestions'][] = 'links__'.$id;
  }
}

function wraith_links__both($variables) {
  $links = $variables['links'];
  $attributes = $variables['attributes'];
  $heading = $variables['heading'];
  global $language_url;
  $output = '';

  if (count($links) > 0) {
    $output = '';

    // Treat the heading first if it is present to prepend it to the
    // list of links.
    if (!empty($heading)) {
      if (is_string($heading)) {
        // Prepare the array that will be used when the passed heading
        // is a string.
        $heading = array(
          'text' => $heading,
          // Set the default level of the heading.
          'level' => 'h2',
        );
      }
      $output .= '<' . $heading['level'];
      if (!empty($heading['class'])) {
        $output .= drupal_attributes(array('class' => $heading['class']));
      }
      $output .= '>' . check_plain($heading['text']) . '</' . $heading['level'] . '>';
    }

    $attributes['class'][] = 'nav';
    $output .= '<ul ' . drupal_attributes($attributes) . '>';

    $num_links = count($links);
    $i = 1;

    foreach ($links as $key => $link) {
      $class = array($key);

      // Add first, last and active classes to the list of links to help out themers.
      if ($i == 1) {
        $class[] = 'first';
      }
      if ($i == $num_links) {
        $class[] = 'last';
      }
      if (isset($link['href']) && ($link['href'] == $_GET['q'] || ($link['href'] == '<front>' && drupal_is_front_page()))
           && (empty($link['language']) || $link['language']->language == $language_url->language)) {
        $class[] = 'active';
      }
      $output .= '<li' . drupal_attributes(array('class' => $class)) . '>';

      if (isset($link['href'])) {
        // Pass in $link as $options, they share the same keys.
        $output .= l($link['title'], $link['href'], $link);
      }
      elseif (!empty($link['title'])) {
        // Some links are actually not links, but we wrap these in <span> for adding title and class attributes.
        if (empty($link['html'])) {
          $link['title'] = check_plain($link['title']);
        }
        $span_attributes = '';
        if (isset($link['attributes'])) {
          $span_attributes = drupal_attributes($link['attributes']);
        }
        $output .= '<span' . $span_attributes . '>' . $link['title'] . '</span>';
      }

      $i++;
      $output .= "</li>\n";
    }

    $output .= '</ul>';
  }

  return $output;
}

/**
* Display a view as a table style.
*/
function wraith_preprocess_views_view_table(&$vars) {
  if(empty($vars['attributes']['class'])){
    $vars['attributes']['class'][] = 'table-striped';
  }
  $vars['classes_array'][] = 'table';
}

function wraith_preprocess_views_view_grid(&$vars) {
  $vars['class'] .= ' table';
}

function wraith_preprocess_views_flipped_table(&$vars) {
  $vars['classes_array'][] = 'table';
}

function wraith_preprocess_table__field_collection_table(&$variables) {
  $variables['attributes']['class'][] = 'table';
  $variables['attributes']['class'][] = 'table-striped';
  $variables['attributes']['class'][] = 'table-bordered';
}



/**
 * Returns HTML for primary and secondary local tasks.
 */
function wraith_menu_local_tasks(&$variables) {
  $output = '';

  if ( !empty($variables['primary']) ) {
    $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $variables['primary']['#prefix'] = '<ul class="nav nav-tabs">';
    $variables['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['primary']);
  }

  if ( !empty($variables['secondary']) ) {
    $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $variables['secondary']['#prefix'] = '<ul class="nav nav-pills">';
    $variables['secondary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['secondary']);
  }

  return $output;
}

function wraith_menu_tree(&$variables) {
  return '<ul class="menu nav">' . $variables['tree'] . '</ul>';
}

function wraith_menu_link(array $variables) {
  $element = $variables['element'];
  $sub_menu = '';

  if ($element['#below']) {
    // Ad our own wrapper
    unset($element['#below']['#theme_wrappers']);
    $sub_menu = '<ul class="dropdown-menu">' . drupal_render($element['#below']) . '</ul>';
    $element['#localized_options']['attributes']['class'][] = 'dropdown-toggle';
    $element['#localized_options']['attributes']['data-toggle'] = 'dropdown';

    // Check if this element is nested within another
    if ((!empty($element['#original_link']['depth'])) && ($element['#original_link']['depth'] > 1)) {
      // Generate as dropdown submenu
      $element['#attributes']['class'][] = 'dropdown-submenu';
    }
    else {
      // Generate as standard dropdown
      $element['#attributes']['class'][] = 'dropdown';
      $element['#localized_options']['html'] = TRUE;
      $element['#title'] .= '<span class="caret"></span>';
    }

    // Set dropdown trigger element to # to prevent inadvertant page loading with submenu click
    $element['#localized_options']['attributes']['data-target'] = '#';
  }

  $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
* Get all primary tasks including subsets
*/
function _wraith_local_tasks($tabs = FALSE) {
  if ($tabs == '') {
    return $tabs;
  }

  if (!$tabs) {
    $tabs = menu_primary_local_tasks();
  }

  foreach ($tabs as $key => $element) {
    $result = db_select('menu_router', NULL, array('fetch' => PDO::FETCH_ASSOC))
      ->fields('menu_router')
      ->condition('tab_parent', $element['#link']['path'])
      ->condition('context', MENU_CONTEXT_INLINE, '<>')
      ->condition('type', array(MENU_DEFAULT_LOCAL_TASK, MENU_LOCAL_TASK), 'IN')
      ->orderBy('weight')
      ->orderBy('title')
      ->execute();

    $router_item = menu_get_item($element['#link']['href']);
    $map = $router_item['original_map'];

    $i = 0;
    foreach ($result as $item) {
      _menu_translate($item, $map, TRUE);

      //only add items that we have access to
      if ($item['tab_parent'] && $item['access']) {
        //set path to that of parent for the first item
        if ($i === 0) {
          $item['href'] = $element['#link']['href'];
        }

        if (current_path() == $item['href']) {
          $tabs[$key][] = array(
          '#theme' => 'menu_local_task',
          '#link' => $item,
          '#active' => TRUE,
          );
        }
        else {
          $tabs[$key][] = array(
          '#theme' => 'menu_local_task',
          '#link' => $item,
          );
        }

        //only count items we have access to.
        $i++;
      }
    }
  }

  return $tabs;
}

function wraith_fieldset($vars) {
  $element = $vars['element'];
  element_set_attributes($element, array('id'));
  _form_set_class($element, array('form-wrapper'));
  if(!empty($element['#collapsible']) && empty($element['#group'])){
    $collapse = empty($element['#collapsed']) ? 'in' : '';
    $title = isset($element['#title']) ? $element['#title'] : 'Toggle';
    $element['#attributes']['class'][] = 'accordion';
    $element['#id'] = empty($element['#id']) ? '' : $element['#id'];
    $element['#id'] = $element['#id'] . rand(10000,99999);
    $output = '<div' . drupal_attributes($element['#attributes']) . '>';
    $output .= ' <div class="accordion-group">';
    $output .= ' <div class="accordion-heading">';
    $output .= ' <a class="accordion-toggle" data-toggle="collapse" data-parent="#'.$element['#id'].'" href="#'.$element['#id'].'-content">'.$title.'</a>';
    $output .= ' </div>';
    $output .= ' <div id="'.$element['#id'].'-content" class="accordion-body collapse in">';
    $output .= ' <div class="accordion-inner">';
    $output .= $element['#children'];
    if (isset($element['#value'])) {
      $output .= $element['#value'];
    }
    $output .= ' </div>';
    $output .= ' </div>';
    $output .= ' </div>';
    $output .= '</div>';
  }else{
    $output = '<fieldset' . drupal_attributes($element['#attributes']) . '>';
    if (!empty($element['#title'])) {
      // Always wrap fieldset legends in a SPAN for CSS positioning.
      if(empty($element['#collapsible'])){
        $output .= '<legend><span class="fieldset-legend">' . $element['#title'] . '</span></legend>';
      }else{
        $output .= '<legend class="js-hide"><span class="fieldset-legend">' . $element['#title'] . '</span></legend>';
      }
    }
    $output .= '<div class="fieldset-wrapper clearfix">';
    if (!empty($element['#description'])) {
      $output .= '<div class="fieldset-description">' . $element['#description'] . '</div>';
    }
    //$output .= '<div class="well well-small">';
    $output .= $element['#children'];
    if (isset($element['#value'])) {
      $output .= $element['#value'];
    }
    //$output .= '</div>';
    $output .= '</div>';
    $output .= "</fieldset>\n";
  }
  return $output;
}
