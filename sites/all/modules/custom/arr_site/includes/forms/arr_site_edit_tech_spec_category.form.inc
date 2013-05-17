<?php

/**
 * Form to add/edit a technical specification category
 */
function arr_site_edit_tech_spec_category($form, &$form_state, $spec_cat_id = 0, $vendor_tid = 0) {
  global $_arr_site;
  
  $vid = !empty($_arr_site['vendors_vocab']) ? $_arr_site['vendors_vocab'] : 0;
  
  $cat = array(
    'spec_cat_id' => $spec_cat_id,
    'tid' => 0,
    'spec_category' => '',
    'weight' => 0,
  );
  
  if (!empty($spec_cat_id) && !empty($vendor_tid)) {
    $form['spec_cat_id'] = array(
      '#type' => 'hidden',
      '#value' => $spec_cat_id,
    );
    
    $sql =
      'SELECT ' .
        'c.`spec_category` AS `spec_category`, ' .
        'ascm.`weight` AS `weight` ' .
      'FROM {arr_spec_categories} c ' .
        'LEFT JOIN {arr_spec_cat_map} ascm ' .
          'ON c.`spec_cat_id` = ascm.`spec_cat_id` ' .
      'WHERE c.`spec_cat_id` = :spec_cat_id ' .
        'AND ascm.`tid` = :tid ' .
      'ORDER BY c.`spec_cat_id` LIMIT 1;';
    $row = db_query($sql, array(':spec_cat_id' => $spec_cat_id, ':tid' => $vendor_tid))->fetchObject();
    
    $cat['spec_category'] = $row->spec_category;
    $cat['weight'] = $row->weight;
    
    $form['tid'] = array(
      '#type' => 'hidden',
      '#value' => $vendor_tid,
    );
    
    $change_all = array(
      '#type' => 'checkbox',
      '#title' => t('Change Weight for all Vendor Types'),
      '#default_value' => 1,
    );
  } else {
    $options = array(
      0 => 'Select one ...',
    );
    
    $options += arr_site_build_list_vendor_categories();
    
    $form['tid'] = array(
      '#type' => 'select',
      '#title' => t('The Vendor Type'),
      '#options' => $options,
      '#default_value' => $cat['tid'],
      '#required' => TRUE,
    );
  }
  
  $form['spec_category'] = array(
    '#type' => 'textfield',
    '#title' => t('Category Name'),
    '#default_value' => $cat['spec_category'],
    '#required' => TRUE,
  );
  
  $form['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Category Weight'),
    '#description' => t('Enter a number greater than or equal to 0. The lower the number the higher in the grid this category will appear. If all category weights are equal they will be organized alphabetically.'),
    '#default_value' => $cat['weight'],
  );
  
  if (!empty($change_all)) {
    $form['change_all'] = $change_all;
  }
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );
  
  $form['#validate'][] = 'arr_site_edit_tech_spec_category_validate';
  $form['#submit'][] = 'arr_site_edit_tech_spec_category_submit';
  
  return $form;
}

/**
 * Validation handler for the arr_site_edit_tech_spec_category form
 */
function arr_site_edit_tech_spec_category_validate(&$form, &$form_state) {
  if (empty($form_state['values']['tid'])) {
    form_set_error('tid', 'You must select a vendor category to proceed.');
  }
  
  $weight_error = FALSE;
  
  if (!is_numeric($form_state['values']['weight'])) {
    $weight_error = TRUE;
  } elseif ($form_state['values']['weight'] < 0) {
    $weight_error = TRUE;
  }
  
  if ($weight_error) {
    form_set_error('weight', 'Only numbers greater than or equal to 0 may be entered for the weight.');
  }
}

/**
 * Submission handler for the arr_site_edit_tech_spec_category form
 */
function arr_site_edit_tech_spec_category_submit(&$form, &$form_state) {
  $cat_table = 'arr_spec_categories';
  $map_table = 'arr_spec_cat_map';
  
  $change_all = !empty($form_state['values']['change_all']) ? $form_state['values']['change_all'] : 0;
  $vendor_tid = $form_state['values']['tid'];
  $weight = !empty($form_state['values']['weight']) ? $form_state['values']['weight'] : 0;
  $sc_id = !empty($form_state['values']['spec_cat_id']) ? $form_state['values']['spec_cat_id'] : 0;
  
  $asc_fields = array('spec_category' => $form_state['values']['spec_category']);
  $ascm_fields = array('weight' => $weight);
  
  $sql =
    'SELECT `name` ' .
    'FROM {taxonomy_term_data} ' .
    'WHERE `tid` = :tid ' .
    'ORDER BY `tid` LIMIT 1';
  $vendor_type = db_query($sql, array(':tid' => $vendor_tid))->fetchField();
  
  $translate_vars = array(
    '!category' => $asc_fields['spec_category'],
    '!vendor_type' => $vendor_type,
  );
  
  // Insert
  if (empty($sc_id)) {
    $spec_cat_id = db_insert($cat_table)
    ->fields($asc_fields)
    ->execute();
    
    $ascm_fields['spec_cat_id'] = $spec_cat_id;
    $ascm_fields['tid'] = $vendor_tid;
    
    if (!empty($spec_cat_id)) {
      $spec_map_id = db_insert($map_table)
        ->fields($ascm_fields)
        ->execute();
    }
    
    $message = t('The new tech spec category, !category, was successfully created and tied to !vendor_type vendors.', $translate_vars); 
  }
  // Update
  else {
    $spec_cat_id = $sc_id;
    
    // Update the category name first
    $num_updated = db_update($cat_table)
      ->fields($asc_fields)
      ->condition('spec_cat_id', $spec_cat_id, '=')
      ->execute();
    
    // Try this way first
    $map_updated = db_update($map_table)
      ->fields($ascm_fields)
      ->condition('spec_cat_id', $spec_cat_id, '=');
    
    if (empty($change_all)) {
      $map_updated->condition('tid', $vendor_tid, '=');
      $message = t('Your changes to the tech spec category, !category, were successfully saved for the !vendor_type vendors, only.', $translate_vars);
    } else {
      $message = t('Your changes to the tech spec category, !category, were successfully saved for all vendors.', array('!category' => $asc_fields['spec_category']));
    }
    
    $map_updated->execute();
    
    // If that fails then this should work
    /*if (!empty($change_all)) {
      $map_updated = db_update($map_table)
        ->fields($ascm_fields)
        ->condition('spec_cat_id', $spec_cat_id, '=')
        ->execute();
      
      $message = t('Your changes to the tech spec category, !category, were successfully saved for the !vendor_type vendors, only.', $translate_vars);
    } else {
      $map_updated = db_update($map_table)
        ->fields($ascm_fields)
        ->condition('spec_cat_id', $spec_cat_id, '=')
        ->condition('tid', $vendor_tid, '=')
        ->execute();
      
      $message = t('Your changes to the tech spec category, !category, were successfully saved for all vendors.', array('!category' => $asc_fields['spec_category']));
    }*/
  }
  
  if (!empty($message)) {
    drupal_set_message($message);
  }
  
  $form_state['redirect'] = 'admin/structure/arr-manage-tech-specs/categories';
}
