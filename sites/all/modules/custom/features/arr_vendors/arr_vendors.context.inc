<?php
/**
 * @file
 * arr_vendors.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function arr_vendors_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'vendors_right_sidebar';
  $context->description = 'Right sidebar blocks for vendor pages.';
  $context->tag = 'vendors';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'vendor' => 'vendor',
      ),
      'options' => array(
        'node_form' => '1',
      ),
    ),
    'user' => array(
      'values' => array(
        'administrator' => 'administrator',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'views-related_content-block' => array(
          'module' => 'views',
          'delta' => 'related_content-block',
          'region' => 'right_col',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 1;

  // Translatables
  // Included for use with string extractors like potx.
  t('Right sidebar blocks for vendor pages.');
  t('vendors');
  $export['vendors_right_sidebar'] = $context;

  return $export;
}
