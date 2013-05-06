<?php
/**
 * @file
 * arr_vendors.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function arr_vendors_taxonomy_default_vocabularies() {
  return array(
    'categories' => array(
      'name' => 'Vendor Categories',
      'machine_name' => 'categories',
      'description' => 'Vendor categories',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '-10',
      'rdf_mapping' => array(
        'rdftype' => array(
          0 => 'skos:ConceptScheme',
        ),
        'name' => array(
          'predicates' => array(
            0 => 'dc:title',
          ),
        ),
        'description' => array(
          'predicates' => array(
            0 => 'rdfs:comment',
          ),
        ),
      ),
    ),
  );
}
