<?php

/**
 * Page callback form for the categories and tech spec list pages
 */
function arr_site_tech_spec_lists($form, &$form_state) {
  global $_arr_site;
  
  // Set the vendor vocabulary VID
  $vid = !empty($_arr_site['vendors_vocab']) ? $_arr_site['vendors_vocab'] : 0;
  
  // If the vendor vocabulary is not set, redirect to the settings page with a message
  if (empty($vid)) {
    drupal_set_message('You must set the vendor vocabulary before proceeding.');
    drupal_goto('admin/structure/arr-manage-tech-specs/settings');
    return;
  }
  
  // Set the list key (categories or specs)
  $list_key = !empty($form_state['build_info']['args'][0]) ? $form_state['build_info']['args'][0] : 'categories';
  // Create a boolean variable to make life easier as we proceed
  $is_cat = ($list_key == 'categories');
  // Create a reusable title variable
  $title_which = ($is_cat) ? 'Categories' : 'Technical Specifications';
  
  // Build the $groups and $cols arrays
  $groups = $cols = array();
  
  // Set the $cols variables and the $sql variable
  if ($is_cat) {
    $cols[] = 'Technical Specification Category';
    
    $sql =
      'SELECT '.
        'c.`spec_cat_id` AS `spec_cat_id`, '.
        'c.`spec_category` AS `spec_category`, '.
        'c.`weight` AS `weight`, '.
        't.`tid` AS `tid`, '.
        't.`name` AS `ven_cat` '.
      'FROM {arr_spec_categories} c '.
        'LEFT JOIN {taxonomy_term_data} t '.
          'ON c.`tid` = t.`tid` '.
      'ORDER BY t.`weight`, t.`name`, c.`weight`, c.`spec_category`;';
  } else {
    $cols[] = 'Technical Specification';
    $cols[] = 'Points if Yes';
    $cols[] = 'Points if No';
    
    $sql =
      'SELECT '.
        's.`spec_id` AS `spec_id`, '.
        's.`spec` AS `spec`, '.
        's.`weight` AS `weight`, '.
        's.`points_yes` AS `points_yes`, '.
        's.`points_no` AS `points_no`, '.
        'c.`spec_cat_id` AS `spec_cat_id`, '.
        'c.`spec_category` AS `spec_category`, '.
        't.`tid` AS `tid`, '.
        't.`name` AS `ven_cat` '.
      'FROM {arr_specs} s '.
        'LEFT JOIN {arr_spec_categories} c '.
          'ON s.`spec_cat_id` = c.`spec_cat_id` '.
        'LEFT JOIN {taxonomy_term_data} t '.
          'ON c.`tid` = t.`tid` '.
      'ORDER BY t.`weight`, t.`name`, c.`weight`, c.`spec_category`, s.`weight`, s.`spec`;';
  }
  
  $cols[] = 'Weight';
  
  // Perform the database query
  $results = db_query($sql);
  
  // Loop through the query results
  while ($row = $results->fetchObject()) {
    // Build a keyed array for this TID if none exists and set the common 'term' variable
    if (!isset($groups[$row->tid])) {
      $groups[$row->tid] = array(
        'term' => $row->ven_cat,
        'rows' => array(),
      );
    }
    
    if ($is_cat) {
      // If this is the categories list, add the row array with the category ID as the array key
      $groups[$row->tid]['rows'][$row->spec_cat_id] = array(
        $row->spec_category,
        $row->weight,
      );
    } else {
      // If this is the specs list, build the category array if none exists
      if (!isset($groups[$row->tid]['rows'][$row->spec_cat_id])) {
        // Each category array should have a category name keyed to 'cat' and a 'rows' array
        $groups[$row->tid]['rows'][$row->spec_cat_id] = array(
          'cat' => $row->spec_category,
          'rows' => array(),
        );
      }
      
      // Add the specs row values keyed to the spec_id
      $groups[$row->tid]['rows'][$row->spec_cat_id]['rows'][$row->spec_id] = array(
        $row->spec,
        $row->points_yes,
        $row->points_no,
        $row->weight,
      );
    }
  }
  
  if (empty($groups)) {
    $form['empty_list'] = array(
      '#markup' => '<p>'.t("Add some @which to get started.", array('@which' => strtolower($title_which))).'</p>',
    );
  } else {
    foreach ($groups as $tid => $data) {
      $tkey = 'term_'.$tid;
      
      $form[$tkey] = array(
        '#type' => 'fieldset',
        '#title' => t("@term", array('@term' => $data['term'])),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      
      if ($is_cat) {
        $form[$tkey]['table_list'] = array(
          '#markup' => _arr_site_tech_spec_list_table($cols, $data['rows'], TRUE),
        );
      } else {
        foreach ($data['rows'] as $spec_cat_id => $d) {
          $ckey = 'cat_'.$spec_cat_id;
          
          $form[$tkey][$ckey] = array(
            '#type' => 'fieldset',
            '#title' => t("@cat", array('@cat' => $d['cat'])),
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
          );
          
          $form[$tkey][$ckey]['table_list'] = array(
            '#markup' => _arr_site_tech_spec_list_table($cols, $d['rows']),
          );
        }
      }
    }
  }
  
  return $form;
}