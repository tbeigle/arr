<?php

/**
 * Form to delete a tech spec category
 */
function arr_site_delete_tech_spec_category($form, &$form_state, $id = 0, $vendor_tid = 0) {
  $sql =
    'SELECT `spec_category` '.
    'FROM {arr_spec_categories} '.
    'WHERE `spec_cat_id` = :spec_cat_id '.
    'ORDER BY `spec_cat_id` LIMIT 1;';
  $row = db_query($sql, array(':spec_cat_id' => $id))->fetchObject();
  
  $form['spec_cat_id'] = array(
    '#type' => 'hidden',
    '#value' => $id,
  );
  
  if (empty($vendor_tid)) {
    $form['confirm_p'] = array(
      '#markup' => '<p>'.t('Are you sure you wish to delete %name? All associated data will be lost.', array('%name' => $row->spec_category)).'</p>',
    );
    
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Delete',
    );
  } else {
    $form['vendor_tid'] = array(
      '#type' => 'hidden',
      '#value' => $vendor_tid,
    );
    
    $form['confirm_p'] = array(
      '#markup' => '<p>'.t('Are you sure you wish to unmap %name from this vendor? All associated data will be lost.', array('%name' => $row->spec_category)).'</p>',
    );
    
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Unmap',
    );
  }
  
  $form['#submit'][] = 'arr_site_delete_tech_spec_category_submit';
  
  return $form;
}

/**
 * Delete a tech spec category
 */
function arr_site_delete_tech_spec_category_submit(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/arr-manage-tech-specs/categories';
  
  $vendor_tid = !empty($form_state['values']['vendor_tid']) ? $form_state['values']['vendor_tid'] : 0;
  $spec_cat_id = $form_state['values']['spec_cat_id'];
  
  $tcids = $spec_ids = array();
  
  $sql =
    'SELECT `tcid`, `tid` ' .
    'FROM {arr_spec_cat_map} ' .
    'WHERE `spec_cat_id` = :spec_cat_id ' .
    'ORDER BY `tid`, `spec_cat_id`;';
  $result = db_query($sql, array(':spec_cat_id' => $spec_cat_id));
  
  foreach ($result as $row) {
    $tcids[$row->tcid] = $row->tid;
  }
  
  // Deleting
  if (empty($vendor_tid) || count($tcids) == 1) {
    // Remove from spec_categories table
    $cat_deleted = db_delete('arr_spec_categories')
      ->condition('spec_cat_id', $spec_cat_id)
      ->execute();
    
    $cat_vendor_rating_deleted = db_delete('arr_spec_cat_vendor_rating')
      ->condition('spec_cat_id', $spec_cat_id);
    
    foreach ($tcids as $tcid => $tid) {
      // Remove from cat_map table
      $cat_map_deleted = db_delete('arr_spec_cat_map')
        ->condition('tcid', $tcid)
        ->execute();
      
      // Grab all the spec_ids
      $sql =
        'SELECT `spec_id` ' .
        'FROM {arr_spec_map} ' .
        'WHERE `tcid` = :tcid ' .
        'ORDER BY `tcid`;';
      $result = db_query($sql, array(':tcid' => $tcid));
      
      foreach ($result as $row) {
        if (!in_array($row->spec_id, $spec_ids)) $spec_ids[] = $row->spec_id;
      }
      
      $spec_map_deleted = db_delete('arr_spec_map')
        ->condition('tcid', $tcid)
        ->execute();
    }
    
    // Delete specs where appropriate
    $delete_specs = array();
    
    foreach ($spec_ids as $spec_id) {
      $sql =
        'SELECT COUNT(*) ' .
        'FROM {arr_spec_map} ' .
        'WHERE `spec_id` = :spec_id ' .
        'ORDER BY `spec_id`;';
      $count = db_query($sql, array(':spec_id' => $spec_id))->fetchField();
      
      if (empty($count)) {
        $delete_specs[] = $spec_id;
      }
    }
    
    foreach ($delete_specs as $spec_id) {
      $spec_vendors_deleted = db_delete('arr_spec_vendors')
        ->condition('spec_id', $spec_id)
        ->execute();
      
      $spec_deleted = db_delete('arr_specs')
        ->condition('spec_id', $spec_id)
        ->execute();
    }
    
    drupal_set_message('The technical specification category was successfully deleted.');
  }
  // Unmapping
  else {
    $tcid_keys = array_keys($tcids, $vendor_tid);
    
    foreach ($tcid_keys as $tcid) {
      $cat_map_deleted = db_delete('arr_spec_cat_map')
        ->condition('tcid', $tcid)
        ->execute();
      
      $spec_map_deleted = db_delete('arr_spec_map')
        ->condition('tcid', $tcid)
        ->execute();
    }
    
    drupal_set_message('The technical specification category was successfully unmapped from the chosen vendor type.');
  }
}
