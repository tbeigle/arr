<?php

/**
 * Form to add/edit a technical specification
 */
function arr_site_edit_tech_spec($form, &$form_state) {
  $id = $form_state['build_info']['args'][0];
  
  $spec = arr_site_get_tech_spec($id);
  
  if (!empty($spec->spec_id)) {
    $form['spec_id'] = array(
      '#type' => 'hidden',
      '#value' => $spec->spec_id,
    );
  }
  
  $options = array(
    '' => 'Select one ...',
  );
  
  $sql =
    'SELECT '.
      'c.`spec_cat_id` AS `spec_cat_id`, '.
      'c.`spec_category` AS `spec_category`, '.
      't.`tid` AS `tid`, '.
      't.`name` AS `ven_cat` '.
    'FROM {arr_spec_categories} c '.
      'LEFT JOIN {taxonomy_term_data} t '.
        'ON c.`tid` = t.`tid` '.
    'ORDER BY t.`weight`, t.`name`, c.`weight`, c.`spec_category`;';
  $results = db_query($sql);
  
  $tids = $cats = array();
  $tids[] = t('Select one ...');
  $tid_default = 0;
  
  while ($row = $results->fetchObject()) {
    if (!isset($tids[$row->tid])) {
      $tids[$row->tid] = $row->ven_cat;
    }
    
    if ($spec->spec_cat_id == $row->spec_cat_id && empty($tid_default)) {
      $tid_default = $row->tid;
    }
    
    if (!isset($cats[$row->tid])) {
      $cats[$row->tid] = array(
        0 => t('Select one ...'),
      );
    }
    
    $cats[$row->tid][$row->spec_cat_id] = $row->spec_category;
  }
  
  $form['tid'] = array(
    '#type' => 'select',
    '#title' => t('Vendor Category'),
    '#options' => $tids,
    '#default_value' => $tid_default,
    '#required' => TRUE,
    '#attributes' => array(
      'class' => array(
        'vendor-tid-select',
      ),
    ),
  );
  
  foreach ($cats as $tid => $ops) {
    $class = array(
      'spec-cat-select',
    );
    
    if (isset($ops[$spec->spec_cat_id]) && !empty($spec->spec_cat_id)) {
      $class[] = 'active-cat';
    }
    
    $form['tid_'.$tid.'_cats'] = array(
      '#type' => 'select',
      '#title' => t('Tech Spec Category'),
      '#options' => $ops,
      '#default_value' => $spec->spec_cat_id,
      '#prefix' => '<div id="cat-wrapper-'.$tid.'" class="'.implode(' ', $class).'">',
      '#suffix' => '</div>',
    );
  }
  
  $form['spec'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $spec->spec,
    '#required' => TRUE,
  );
  
  $form['points_yes'] = array(
    '#type' => 'textfield',
    '#title' => t('Points to Add/Subtract if Vendor has the Spec'),
    '#default_value' => $spec->points_yes,
    '#description' => t('Enter a positive or a negative integer to indicate how many points should be added or subtracted to the category score if the vendor has this tech spec. Enter 0 to neither add nor subtract points.'),
  );
  
  $form['points_no'] = array(
    '#type' => 'textfield',
    '#title' => t('Points to Add/Subtract if Vendor does not have the Spec'),
    '#default_value' => $spec->points_no,
    '#description' => t('Enter a positive or a negative integer to indicate how many points should be added or subtracted to the category score if the vendor does not have this tech spec. Enter 0 to neither add nor subtract points.'),
  );
  
  $form['weight'] = array(
    '#type' => 'textfield',
    '#title' => t('Tech Spec Weight'),
    '#description' => t('Enter a number greater than or equal to 0. The lower the number the higher in the grid this tech spec will appear. If all tech spec weights are equal they will be organized alphabetically.'),
    '#default_value' => !empty($spec->weight) ? $spec->weight : 0,
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  
  $form['#validate'][] = 'arr_site_validate_tech_spec';
  $form['#submit'][] = 'arr_site_submit_tech_spec';
  
  return $form;
}

/**
 * edit_tech_spec form validation
 */
function arr_site_validate_tech_spec(&$form, &$form_state) {
  if (!is_numeric($form_state['values']['weight'])) {
    form_set_error('weight', 'Pleaser enter a number greater than or equal to 0.');
  }
  
  if (!empty($form_state['values']['tid'])) {
    $key = 'tid_'.$form_state['values']['tid'].'_cats';
    
    if (empty($form_state['values'][$key])) {
      form_set_error('', 'You must select a tech spec category to proceed.');
    }
  }
  
  if (empty($form_state['values']['points_yes'])) $form_state['values']['points_yes'] = 0;
  if (empty($form_state['values']['points_no'])) $form_state['values']['points_no'] = 0;
  
  if (!is_numeric($form_state['values']['points_yes'])) {
    form_set_error('points_yes', 'Only integers are allowed in this field.');
  }
  
  if (!is_numeric($form_state['values']['points_no'])) {
    form_set_error('points_no', 'Only integers are allowed in this field.');
  }
}

/**
 * edit_tech_spec form submission
 */
function arr_site_submit_tech_spec(&$form, &$form_state) {
  $options = array();
  
  $key = 'tid_'.$form_state['values']['tid'].'_cats';
  
  $fields = array(
    'spec_cat_id' => $form_state['values'][$key],
    'spec' => $form_state['values']['spec'],
    'points_yes' => $form_state['values']['points_yes'],
    'points_no' => $form_state['values']['points_no'],
    'weight' => $form_state['values']['weight'],
  );
  
  if (empty($form_state['values']['spec_id'])) {
    $spec_id = db_insert('arr_specs')
    ->fields($fields)
    ->execute();
  } else {
    $num_updated = db_update('arr_specs')
    ->fields($fields)
    ->condition('spec_id', $form_state['values']['spec_id'], '=')
    ->execute();
  }
  
  drupal_set_message('Your changes were successfully saved.');
  $form_state['redirect'] = 'admin/structure/arr-manage-tech-specs/specs';
}