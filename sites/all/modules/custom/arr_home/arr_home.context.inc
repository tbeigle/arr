<?php
/**
 * @file
 * arr_home.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function arr_home_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'home';
  $context->description = 'Homepage blocks';
  $context->tag = 'theme';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        '<front>' => '<front>',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'menu-menu-popular-categories' => array(
          'module' => 'menu',
          'delta' => 'menu-popular-categories',
          'region' => 'content_bottom',
          'weight' => '-9',
        ),
        'arr_vendors-front_vendors' => array(
          'module' => 'arr_vendors',
          'delta' => 'front_vendors',
          'region' => 'content_bottom',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 1;

  // Translatables
  // Included for use with string extractors like potx.
  t('Homepage blocks');
  t('theme');
  $export['home'] = $context;

  return $export;
}
