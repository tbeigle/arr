<?php

/**
 * Implements hook_form_alter()
 */
function arr_site_form_alter(&$form, &$form_state, $form_id) {
  global $_arr_site, $user;
  
  $vid = !empty($_arr_site['vendors_vocab']) ? $_arr_site['vendors_vocab'] : 0;
  
  if (!empty($form['submitted']['are_you_a_vendor_representative']) && !empty($form['submitted']['vendor_details'])) {
    if (!isset($_arr_site['vendor_details_toggle_count'])) $_arr_site['vendor_details_toggle_count'] = 0;
    
    $toggle_class = 'details-collapsible-'.$_arr_site['vendor_details_toggle_count'];
    
    $field_classes = array(
      'are_you_a_vendor_representative' => 'toggle-details',
      'vendor_details' => $toggle_class,
    );
    
    foreach ($field_classes as $key => $class) {
      if (empty($form['submitted'][$key]['#attributes'])) {
        $form['submitted'][$key]['#attributes'] = array();
      }
      
      if (empty($form['submitted'][$key]['#attributes']['class'])) {
        $form['submitted'][$key]['#attributes']['class'] = array();
      }
      
      $form['submitted'][$key]['#attributes']['class'][] = $class;
    }
    
    $form['submitted']['are_you_a_vendor_representative']['#attributes']['data-to-toggle'] = '.'.$toggle_class;
    
    $_arr_site['vendor_details_toggle_count']++;
  }
  
  switch ($form_id) {
    case 'webform_client_form_12': {
    }
    case 'comment_node_vendor_form': {
      if (empty($user->uid)) {
        $form['arr_username'] = array(
          '#type' => 'textfield',
          '#title' => t('Username'),
          '#attributes' => array(
            'placeholder' => 'Username',
          ),
        );
        
        $form['arr_password'] = array(
          '#type' => 'password',
          '#title' => t('Password'),
          '#attributes' => array(
            'placeholder' => 'Password',
          ),
          '#prefix' => '<div id="arr-comment-password-wrapper" class="clearfix">',
          '#suffix' => '<div class="join-arr"><a href="/user/register">Not a ARR Member? Join and leave a review.</a></div></div>',
        );
        
        $form['q_val'] = array(
          '#type' => 'hidden',
          '#value' => $_GET['q'],
        );
        
        $form['#validate'][] = 'arr_site_comment_validate';
        $form['#submit'][] = 'arr_site_comment_submit';
      }
      
      break;
    }
  }
}

/**
 * Custom validation function for the comment form
 */
function arr_site_comment_validate(&$form, &$form_state) {
  global $user;
  
  $q = !empty($form_state['values']['q_val']) ? $form_state['values']['q_val'] : '';
  
  if (!empty($q)) {
    $form_state['redirect'] = $q;
  }
  
  $name = !empty($form_state['values']['arr_username']) ? $form_state['values']['arr_username'] : '';
  $pass = !empty($form_state['values']['arr_password']) ? $form_state['values']['arr_password'] : '';
  $uid = user_authenticate($name, $pass);
  
  $form_state['uid'] = $uid;
  
  if ($uid === FALSE) {
    form_set_error('arr_password', 'The username/password combination you entered is invalid.');
  } else {
    $user = user_load($form_state['uid']);
    user_login_finalize($form_state);
    
    foreach ($form_state['values'] as $key => $val) {
      if (!empty($user->{$key})) {
        $form_state['values'][$key] = $user->{$key};
      }
    }
  }
}

/**
 * Custom submit handler for the comment form
 */
function arr_site_comment_submit(&$form, &$form_state) {
}