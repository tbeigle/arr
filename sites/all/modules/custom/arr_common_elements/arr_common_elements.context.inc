<?php
/**
 * @file
 * arr_common_elements.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function arr_common_elements_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'bubble_links';
  $context->description = '';
  $context->tag = 'theme';
  $context->conditions = array(
    'user' => array(
      'values' => array(
        'administrator' => 'administrator',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'menu-menu-front-bubbles' => array(
          'module' => 'menu',
          'delta' => 'menu-front-bubbles',
          'region' => 'footer_top',
          'weight' => '-10',
        ),
        'webform-client-block-12' => array(
          'module' => 'webform',
          'delta' => 'client-block-12',
          'region' => 'popups',
          'weight' => '-10',
        ),
        'webform-client-block-13' => array(
          'module' => 'webform',
          'delta' => 'client-block-13',
          'region' => 'popups',
          'weight' => '-9',
        ),
      ),
    ),
  );
  $context->condition_mode = 1;

  // Translatables
  // Included for use with string extractors like potx.
  t('theme');
  $export['bubble_links'] = $context;

  return $export;
}
