<?php

/**
 * Form to configure tech spec settings
 */
function arr_site_edit_tech_spec_settings($form, &$form_state) {
  global $_arr_site;
  
  $type = !empty($_arr_site['vendor_type']) ? $_arr_site['vendor_type'] : '';
  $vid = !empty($_arr_site['vendors_vocab']) ? $_arr_site['vendors_vocab'] : 0;
  
  // Vendor node type
  $options = array(
    0 => 'Choose one ...',
  );
  
  $sql =
    'SELECT `type`, `name` '.
    'FROM {node_type} '.
    'ORDER BY `name`;';
  $results = db_query($sql);
  
  while ($row = $results->fetchObject()) {
    $options[$row->type] = $row->name;
  }
  
  $form['node_type'] = array(
    '#type' => 'select',
    '#title' => t('Vendor Content Type'),
    '#description' => t('Select the content type currently applied to vendor nodes.'),
    '#options' => $options,
    '#default_value' => $type,
    '#required' => TRUE,
  );
  
  // Vendor category taxonomy vocabulary
  $options = array(
    0 => 'Choose one ...',
  );
  
  $sql =
    'SELECT `vid`, `name` '.
    'FROM {taxonomy_vocabulary} '.
    'ORDER BY `name`;';
  $results = db_query($sql);
  
  while ($row = $results->fetchObject()) {
    $options[$row->vid] = $row->name;
  }
  
  $form['vid'] = array(
    '#type' => 'select',
    '#title' => t('Vendor Category Vocabulary'),
    '#description' => t('Select the vendor categories vocabulary.'),
    '#options' => $options,
    '#default_value' => $vid,
    '#required' => TRUE,
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  
  $form['#submit'][] = 'arr_site_edit_tech_spec_settings_submit';
  
  return $form;
}

/**
 * form submission handler for the tech spec configuration form
 */
function arr_site_edit_tech_spec_settings_submit(&$form, &$form_state) {
  variable_set('arr_site_vendor_node_type', $form_state['values']['node_type']);
  variable_set('arr_site_vendor_vocabulary', $form_state['values']['vid']);
}