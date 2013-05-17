<?php

/**
 * Form to add/edit a technical specification
 */
function arr_site_edit_tech_spec($form, &$form_state, $id = 0, $vendor_tid = 0) {
  $spec = arr_site_get_tech_spec($vendor_tid, $id);
  
  if (!empty($spec->spec_id)) {
    $form['spec_id'] = array(
      '#type' => 'hidden',
      '#value' => $spec->spec_id,
    );
  }
  
  $options = array(
    '' => 'Select one ...',
  );
  
  if (empty($vendor_tid)) {
    $sql =
      'SELECT '.
        'c.`spec_cat_id` AS `spec_cat_id`, ' .
        'c.`spec_category` AS `spec_category`, ' .
        't.`tid` AS `tid`, ' .
        't.`name` AS `ven_cat` ' .
      'FROM {arr_spec_categories} c ' .
        'LEFT JOIN {arr_spec_cat_map} ascm ' .
          'ON c.`spec_cat_id` = ascm.`spec_cat_id` ' .
        'LEFT JOIN {taxonomy_term_data} t ' .
          'ON ascm.`tid` = t.`tid` '.
      'ORDER BY t.`weight`, t.`name`, ascm.`weight`, c.`spec_category`;';
    $result = db_query($sql);
  } else {
    $sql =
      'SELECT '.
        'c.`spec_cat_id` AS `spec_cat_id`, ' .
        'c.`spec_category` AS `spec_category`, ' .
        't.`tid` AS `tid`, ' .
        't.`name` AS `ven_cat` ' .
      'FROM {arr_spec_categories} c ' .
        'LEFT JOIN {arr_spec_cat_map} ascm ' .
          'ON c.`spec_cat_id` = ascm.`spec_cat_id` ' .
        'LEFT JOIN {taxonomy_term_data} t ' .
          'ON ascm.`tid` = t.`tid` '.
      'WHERE ascm.`tid` = :tid ' .
      'ORDER BY t.`weight`, t.`name`, ascm.`weight`, c.`spec_category`;';
    $result = db_query($sql, array(':tid' => $vendor_tid));
  }
  
  $tids = $cats = array();
  $tids[] = t('Select one ...');
  $tid_default = 0;
  
  foreach ($result as $row) {
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
  
  if (!empty($vendor_tid)) {
    $form['tid'] = array(
      '#type' => 'hidden',
      '#value' => $vendor_tid,
    );
    
    if (!empty($spec->spec_id)) {
      $sql =
        'SELECT ' .
          'ascm.`tcid` AS `tcid`, ' .
          'asm.`tcsid` AS `tcsid` ' .
        'FROM {arr_spec_cat_map} ascm ' .
          'LEFT JOIN {arr_spec_map} asm ' .
            'ON ascm.`tcid` = asm.`tcid` ' .
        'WHERE ascm.`tid` = :tid ' .
          'AND asm.`spec_id` = :spec_id ' .
        'ORDER BY asm.`spec_id`, ascm.`tid` LIMIT 1;';
      $spec_data = db_query($sql, array(':tid' => $vendor_tid, ':spec_id' => $spec->spec_id))->fetchObject();
      
      if (!empty($spec_data->tcid)) {
        $form['tcid'] = array(
          '#type' => 'hidden',
          '#value' => $spec_data->tcid,
        );
      }
      
      if (!empty($spec_data->tcsid)) {
        $form['tcsid'] = array(
          '#type' => 'hidden',
          '#value' => $spec_data->tcsid,
        );
      }
    }
    
    $change_all = array(
      '#type' => 'checkbox',
      '#title' => t('Change Spec Data for All Vendor Types'),
      '#default_value' => 1,
    );
  } else {
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
  }
  
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
  
  if (!empty($change_all)) {
    $form['change_all'] = $change_all;
  }
  
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
  
  $tid = !empty($form_state['values']['tid']) ? $form_state['values']['tid'] : 0;
  $key = 'tid_'.$tid.'_cats';
  $spec_cat_id = $form_state['values'][$key];
  $cur_tcid = !empty($form_state['values']['tcid']) ? $form_state['values']['tcid'] : 0;
  $tcsid = !empty($form_state['values']['tcsid']) ? $form_state['values']['tcsid'] : 0;
  $sid = !empty($form_state['values']['spec_id']) ? $form_state['values']['spec_id'] : 0;
  $is_new = empty($sid);
  $change_all = !empty($form_state['values']['change_all']);
  $spec = !empty($form_state['values']['spec']) ? $form_state['values']['spec'] : '';
  $points_yes = !empty($form_state['values']['points_yes']) ? $form_state['values']['points_yes'] : 0;
  $points_no = !empty($form_state['values']['points_no']) ? $form_state['values']['points_no'] : 0;
  $weight = !empty($form_state['values']['weight']) ? $form_state['values']['weight'] : 0;
  
  $spec_fields = array('spec' => $spec);
  $map_fields = array(
    'points_yes' => $points_yes,
    'points_no' => $points_no,
    'weight' => $weight,
  );
  
  $sql =
    'SELECT `tcid` ' .
    'FROM {arr_spec_cat_map} ' .
    'WHERE `tid` = :tid ' .
      'AND `spec_cat_id` = :spec_cat_id ' .
    'ORDER BY `tid`, `spec_cat_id` LIMIT 1;';
  $tcid = db_query($sql, array(':tid' => $tid, ':spec_cat_id' => $spec_cat_id))->fetchField();
  
  $change_tcid = (!empty($cur_tcid) && !empty($tcid) && $cur_tcid != $tcid);
  
  if ($change_tcid || $is_new) {
    $map_fields['tcid'] = $tcid;
  }
  
  if ($is_new) {
    $map_fields['spec_id'] = $spec_id = db_insert('arr_specs')
      ->fields($spec_fields)
      ->execute();
    
    if (!empty($map_fields['spec_id'])) {
      $tcsid = db_insert('arr_spec_map')
        ->fields($map_fields)
        ->execute();
    }
  } else {
    $spec_id = $sid;
    
    $num_updated = db_update('arr_specs')
      ->fields($spec_fields)
      ->condition('spec_id', $spec_id)
      ->execute();
    
    if ($change_all) {
      if (!empty($map_fields['tcid'])) {
        // Only update this spec record's tcid field
        $map_updated = db_update('arr_spec_map')
          ->fields(array(
            'tcid' => $map_fields['tcid'],
          ))
          ->condition('tcsid', $tcsid, '=')
          ->execute();
        
        // Unset the tcid field so we don't screw up other spec instances
        unset($map_fields['tcid']);
      }
      
      // Update the remaining fields for all instances of this spec
      $map_updated_all = db_update('arr_spec_map')
        ->fields($map_fields)
        ->condition('spec_id', $spec_id, '=')
        ->execute();
    } else {
      $map_updated = db_update('arr_spec_map')
        ->fields($map_fields)
        ->condition('tcsid', $tcsid, '=')
        ->execute();
    }
  }
  
  drupal_set_message('Your changes were saved.');
  $form_state['redirect'] = 'admin/structure/arr-manage-tech-specs/specs';
}
