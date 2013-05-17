<?php

/**
 * Page callback form for the tech specs category cloning
 */
function arr_site_clone_tech_spec_category($form, &$form_state, $tid = 0, $func = 'clone') {
  global $_arr_site;
  
  if (empty($_arr_site['vendors_vocab'])) return array();
  
  $form = $options = array();
  
  $form['clone_me'] = array(
    '#type' => 'hidden',
    '#value' => $tid,
  );
  
  $form['func'] = array(
    '#type' => 'hidden',
    '#value' => $func,
  );
  
  $options = arr_site_build_list_vendor_categories($tid);
  
  $form['tid'] = array(
    '#type' => 'select',
    '#title' => t('New Term'),
    '#options' => $options,
  );
  
  if ($func == 'map') {
    $form['tid']['#multiple'] = TRUE;
  }
  
  // Upon submit, clone all the tech specs and tie them to the selected category
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => ucwords($func),
  );
  
  $form['#submit'][] = 'arr_site_clone_tech_spec_category_submit';
  
  return $form;
}

/**
 * Maps

/**
 * Submit handler for the tech spec category cloning form
 */
function arr_site_clone_tech_spec_category_submit(&$form, &$form_state) {
  $groups = array();
  $to_clone_tid = $form_state['values']['clone_me'];
  $clone_tid = $form_state['values']['tid'];
  
  $func = $form_state['values']['func'];
  
  // Map
  if ($func == 'map') {
    arr_site_map_new_vendor($to_clone_tid, $clone_tid);
    $did = 'mapped';
  }
  // Clone
  elseif ($func == 'clone') {
    arr_site_clone_vendor($to_clone_tid, $clone_tid);
    $did = 'cloned';
  }
  
  $message = t('Successully !did existing vendor categories and tech specs to the new vendor type.', array('!did' => $did));
  drupal_set_message($message);
  $form_state['redirect'] = 'admin/structure/arr-manage-tech-specs/categories';
}
