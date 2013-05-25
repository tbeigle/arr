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
  
  arr_site_delete_category_by_spec_cat_id($spec_cat_id, $vendor_tid);
}
