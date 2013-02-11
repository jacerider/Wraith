<?php

function theme_wraith_aggregate_js_files($vars) {
  $element = $vars['element'];
  drupal_add_tabledrag('trip-sort', 'order', 'sibling', 'rank-weight'); // needed for table dragging

  $header = array(
    'markup' => '',
    'weight' => '',
  );

  $rows = array();
  foreach (element_children($element) as $key) {
    $row = array();

    $row['data'] = array();
    foreach ($header as $fieldname => $title) {
      $row['data'][] = drupal_render($element[$key][$fieldname]);
      $row['class'] = array('draggable'); // needed for table dragging
    }
    $rows[] = $row;
  }

  $output .= theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'trip-sort'), // needed for table dragging
  ));

  return $output;
}
