<?php

/**
 * Form to add/edit a technical specification category
 */
function arr_site_edit_tech_spec_category($form, &$form_state) {
  global $_arr_site;
  
  $vid = !empty($_arr_site['vendors_vocab']) ? $_arr_site['vendors_vocab'] : 0;
  
  $cat = array(
    'spec_cat_id' => !empty($form_state['build_info']['args'][0]) ? $form_state['build_info']['args'][0] : 0,
    'tid' => 0,
    'spec_category' => '',
    'weight' => 0,
  );
  
  if (!empty($cat['spec_cat_id'])) {
    $form['spec_cat_id'] = array(
      '#type' => 'hidden',
      '#value' => $cat['spec_cat_id'],
    );
    
    $sql =
      'SELECT `tid`, `spec_category`, `weight` '.
      'FROM {arr_spec_categories} '.
      'WHERE `spec_cat_id` = :spec_cat_id '.
      'ORDER BY `spec_cat_id` LIMIT 1;';
    $row = db_query($sql, array(':spec_cat_id' => $cat['spec_cat_id']))->fetchObject();
    
    $cat['tid'] = $row->tid;
    $cat['spec_category'] = $row->spec_category;
    $cat['weight'] = $row->weight;
  }
  
  $options = array(
    0 => 'Select one ...',
  );
  
  $sql =
    'SELECT `tid`, `name` '.
    'FROM {taxonomy_term_data} '.
    'WHERE `vid` = :vid '.
    'ORDER BY `vid`, `name`;';
  $results = db_query($sql, array(':vid' => $vid));
  
  while ($row = $results->fetchObject()) {
    $options[$row->tid] = $row->name;
  }
  
  $form['tid'] = array(
    '#type' => 'select',
    '#title' => t('The Vendor Category'),
    '#options' => $options,
    '#default_value' => $cat['tid'],
    '#required' => TRUE,
  );
  
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
  $fields = array(
    'tid' => $form_state['values']['tid'],
    'spec_category' => $form_state['values']['spec_category'],
    'weight' => !empty($form_state['values']['weight']) ? $form_state['values']['weight'] : 0,
  );
  
  if (empty($form_state['values']['spec_cat_id'])) {
    $spec_cat_id = db_insert('arr_spec_categories')
    ->fields($fields)
    ->execute();
  } else {
    $num_updated = db_update('arr_spec_categories')
    ->fields($fields)
    ->condition('spec_cat_id', $form_state['values']['spec_cat_id'], '=')
    ->execute();
  }
  
  $form_state['redirect'] = 'admin/structure/arr-manage-tech-specs/categories';
}