<?php

/**
 * Form to delete a tech spec category
 */
function arr_site_delete_tech_spec_category($form, &$form_state) {
  $id = $form_state['build_info']['args'][0];
  
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
  
  $form['confirm_p'] = array(
    '#markup' => '<p>'.t('Are you sure you wish to delete %name?', array('%name' => $row->spec_category)).'</p>',
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Delete',
  );
  
  $form['#submit'][] = 'arr_site_delete_tech_spec_category_submit';
  
  return $form;
}

/**
 * Delete a tech spec category
 */
function arr_site_delete_tech_spec_category_submit(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/arr-manage-tech-specs/categories';
  
  $num_deleted = db_delete('arr_spec_categories')
  ->condition('spec_cat_id', $form_state['values']['spec_cat_id'])
  ->execute();
  
  drupal_set_message('The technical specification was successfully deleted.');
}