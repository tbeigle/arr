<?php

/**
 * @file
 * Theme setting callbacks for the All Rave Reviews theme.
 */

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * @param &$form
 *   The form.
 * @param &$form_state
 *   The form state.
 */
function arr_form_system_theme_settings_alter(&$form, &$form_state) {
  // Enable/Disable homepage slideshow for anonymous visitors
  $form['arr_enable_home_ss'] = array(
    '#type' => 'radios',
    '#title' => 'Enable Slideshows',
    '#options' => array(
      0 => t('For Admins Only'),
      1 => t('For All Visitors'),
    ),
    '#default_value' => theme_get_setting('arr_enable_home_ss'),
  );
}
