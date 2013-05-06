<?php

/**
 * Form to delete a tech spec
 */
function arr_site_delete_tech_spec($form, &$form_state) {
  $id = $form_state['build_info']['args'][0];
  
  $spec = arr_site_get_tech_spec($id);
  
  $form['spec_id'] = array(
    '#type' => 'hidden',
    '#value' => $id,
  );
  
  $form['confirm_p'] = array(
    '#markup' => '<p>'.t('Are you sure you wish to delete %name?', array('%name' => $spec->spec)).'</p>',
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Delete',
  );
  
  $form['#submit'][] = 'arr_site_delete_tech_spec_submit';
  
  return $form;
}

/**
 * Delete a tech spec form submission handler
 */
function arr_site_delete_tech_spec_submit(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/arr-manage-tech-specs/specs';
  
  $num_deleted = db_delete('arr_specs')
  ->condition('spec_id', $form_state['values']['spec_id'])
  ->execute();
  
  drupal_set_message('The technical specification was successfully deleted.');
}