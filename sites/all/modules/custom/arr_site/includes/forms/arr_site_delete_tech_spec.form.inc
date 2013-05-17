<?php

/**
 * Form to delete a tech spec
 */
function arr_site_delete_tech_spec($form, &$form_state, $id = 0, $vendor_tid = 0) {
  $spec = arr_site_get_tech_spec($id);
  
  $form['spec_id'] = array(
    '#type' => 'hidden',
    '#value' => $id,
  );
  
  if (empty($vendor_tid)) {
    $form['confirm_p'] = array(
      '#markup' => '<p>'.t('Are you sure you wish to delete %name?', array('%name' => $spec->spec)).'</p>',
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
      '#markup' => '<p>'.t('Are you sure you wish to unmap %name from this vendor type?', array('%name' => $spec->spec)).'</p>',
    );
    
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Unmap',
    );
  }
  
  $form['#submit'][] = 'arr_site_delete_tech_spec_submit';
  
  return $form;
}

/**
 * Delete a tech spec form submission handler
 */
function arr_site_delete_tech_spec_submit(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/arr-manage-tech-specs/specs';
  
  $vendor_tid = !empty($form_state['values']['vendor_tid']) ? $form_state['values']['vendor_tid'] : 0;
  $spec_id = $form_state['values']['spec_id'];
  
  // Delete
  if (empty($vendor_tid)) {
    $spec_deleted = db_delete('arr_specs')
      ->condition('spec_id', $spec_id)
      ->execute();
    
    $spec_map_deleted = db_delete('arr_spec_map')
      ->condition('spec_id', $spec_id)
      ->execute();
    
    $spec_vendors_deleted = db_delete('arr_spec_vendors')
      ->condition('spec_id', $spec_id)
      ->execute();
    
    drupal_set_message('The technical specification was successfully deleted.');
  }
  // Unmap
  else {
    $tcids = array();
    
    $sql =
      'SELECT ascm.`tcid` AS `tcid` ' .
      'FROM {arr_spec_cat_map} ascm ' .
        'LEFT JOIN {arr_spec_map} asm ' .
          'ON ascm.`tcid` = asm.`tcid` ' .
      'WHERE ascm.`tid` = :tid ' .
        'AND asm.`spec_id` = :spec_id ' .
      'ORDER BY ascm.`tid`, asm.`spec_id`;';
    $result = db_query($sql, array(':tid' => $vendor_tid, ':spec_id' => $spec_id));
    
    foreach ($result as $row) {
      $tcids[] = $row->tcid;
    }
    
    foreach ($tcids as $tcid) {
      $spec_map_deleted = db_delete('arr_spec_map')
        ->condition('spec_id', $spec_id)
        ->condition('tcid', $tcid)
        ->execute();
    }
    
    drupal_set_message('The technical specification was successfully unmapped from the specified vendor type.');
  }
}
