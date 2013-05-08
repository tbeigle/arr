<?php

/**
 * Form to assign tech specs to a vendor node
 */
function arr_site_edit_node_tech_specs($form, &$form_state) {
  $nid = $form_state['build_info']['args'][0];
  $node = node_load($nid);
  
  $tids = array();
  
  if (!empty($node->field_vendor_categories[LANGUAGE_NONE])) {
    foreach ($node->field_vendor_categories[LANGUAGE_NONE] as $key => $data) {
      if (!empty($data['tid'])) {
        $tids[] = $data['tid'];
      }
    }
  }
  
  $specs = array();
  
  $sql =
    'SELECT `spec_id`, `has_spec`, `spec_comment` '.
    'FROM {arr_spec_vendors} '.
    'WHERE `nid` = :nid '.
    'ORDER BY `nid`;';
  $results = db_query($sql, array(':nid' => $nid));
  
  while ($row = $results->fetchObject()) {
    $specs[$row->spec_id] = array(
      'has_spec' => $row->has_spec,
      'spec_comment' => $row->spec_comment
    );
  }
  
  $groups = array();
  
  $sql =
    'SELECT '.
      's.`spec_id` AS `spec_id`, '.
      's.`spec` AS `spec`, '.
      'c.`spec_cat_id` AS `spec_cat_id`, '.
      'c.`spec_category` AS `spec_category`, '.
      't.`tid` AS `tid`, '.
      't.`name` AS `ven_cat` '.
    'FROM {arr_specs} s '.
      'LEFT JOIN {arr_spec_categories} c '.
        'ON s.`spec_cat_id` = c.`spec_cat_id` '.
      'LEFT JOIN {taxonomy_term_data} t '.
        'ON c.`tid` = t.`tid` '.
    'ORDER BY t.`weight`, t.`name`, c.`weight`, c.`spec_category`, s.`weight`, s.`spec`;';
  $results = db_query($sql);
  
  while ($row = $results->fetchObject()) {
    if (!isset($row->tid)) continue;
    if (!in_array($row->tid, $tids)) continue;
    
    if (!isset($groups[$row->tid])) {
      $groups[$row->tid] = array(
        'term' => $row->ven_cat,
        'rows' => array(),
      );
    }
    
    if (!isset($groups[$row->tid]['rows'][$row->spec_cat_id])) {
      $groups[$row->tid]['rows'][$row->spec_cat_id] = array(
        'cat' => $row->spec_category,
        'rows' => array(),
      );
    }
    
    $groups[$row->tid]['rows'][$row->spec_cat_id]['rows'][$row->spec_id] = $row->spec;
  }
  
  $form['specs'] = array(
    '#tree' => TRUE,
  );
  
  $cat_ratings = array();
  
  $sql =
    'SELECT `spec_cat_id`, `rating` '.
    'FROM {arr_spec_cat_vendor_rating} '.
    'WHERE `nid` = :nid '.
    'ORDER BY `nid`;';
  $results = db_query($sql, array(':nid' => $nid));
  
  while ($row = $results->fetchObject()) {
    $cat_ratings[$row->spec_cat_id] = $row->rating;
  }
  
  foreach ($groups as $tid => $data) {
    $tkey = 'term_'.$tid;
    
    $form['specs'][$tkey] = array(
      '#type' => 'fieldset',
      '#title' => t("@term", array('@term' => $data['term'])),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    
    foreach ($data['rows'] as $ckey => $d) {
      $form['specs'][$tkey][$ckey] = array(
        '#type' => 'fieldset',
        '#title' => t("@cat Tech Specs", array('@cat' => $d['cat'])),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      
      $form['specs'][$tkey][$ckey]['rating'] = array(
        '#type' => 'select',
        '#title' => t('ARR Star Rating for this Vendor in this Category'),
        '#options' => array(
          0 => '0 Stars',
          1 => '1 Star',
          2 => '2 Stars',
          3 => '3 Stars',
          4 => '4 Stars',
          5 => '5 Stars',
        ),
        '#default_value' => !empty($cat_ratings[$ckey]) ? $cat_ratings[$ckey] : 0,
      );
      
      $form['specs'][$tkey][$ckey]['has_rating'] = array(
        '#type' => 'hidden',
        '#value' => isset($cat_ratings[$ckey]) ? 1 : 0,
      );
      
      $form['specs'][$tkey][$ckey]['table_start'] = array(
        '#markup' =>
          '<table>'.
            '<thead>'.
              '<tr>'.
                '<th>'.t('Tech Spec').'</th>'.
                '<th>'.t('Does the vendor feature this tech spec?').'</th>'.
                '<th>'.t('Comments Specific to this Vendor and Tech Spec').'</th>'.
              '</tr>'.
            '</thead>'.
            '<tbody>',
      );
      
      foreach ($d['rows'] as $spec_id => $spec) {
        $form['specs'][$tkey][$ckey][$spec_id] = array(
          '#tree' => TRUE,
        );
        
        $form['specs'][$tkey][$ckey][$spec_id]['has_spec'] = array(
          '#type' => 'radios',
          '#title' => t('Vendor has the Spec'),
          '#title_display' => 'invisible',
          '#options' => array(
            0 => 'No, the vendor does not have this tech spec.',
            1 => 'Yes, the vendor has this tech spec.',
          ),
          '#default_value' => !empty($specs[$spec_id]['has_spec']) ? $specs[$spec_id]['has_spec'] : 0,
          '#prefix' => '<tr><td>'.$spec.'</td><td>',
          '#suffix' => '</td>',
        );
        
        $form['specs'][$tkey][$ckey][$spec_id]['spec_comment'] = array(
          '#type' => 'textarea',
          '#title' => 'Comments',
          '#title_display' => 'invisible',
          '#default_value' => !empty($specs[$spec_id]['spec_comment']) ? $specs[$spec_id]['spec_comment'] : '',
          '#cols' => 30,
          '#rows' => 2,
          '#prefix' => '<td>',
          '#suffix' => '</td></tr>',
        );
      }
      
      $form['specs'][$tkey][$ckey]['table_end'] = array(
        '#markup' => '</tbody></table>',
      );
    }
  }
  
  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $nid,
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  
  $form['#submit'][] = 'arr_site_edit_node_tech_specs_submit';
  
  return $form;
}

/**
 * Form to assign tech specs to a vendor node
 */
function arr_site_edit_node_tech_specs_submit(&$form, &$form_state) {
  $fv = (object) $form_state['values'];
  $nid = $fv->nid;
  
  $num_deleted = db_delete('arr_spec_vendors')
  ->condition('nid', $nid)
  ->execute();
  
  foreach ($fv->specs as $term_key => $cat_data) {
    foreach ($cat_data as $cat_key => $spec_data) {
      $rating = array(
        'has_rating' => 0,
        'rating' => 0,
      );
      
      foreach ($rating as $rkey => $rval) {
        if (isset($spec_data[$rkey])) {
          $rating[$rkey] = $spec_data[$rkey];
          unset($spec_data[$rkey]);
        }
      }
      
      if (!empty($rating['has_rating'])) {
        $num_updated = db_update('arr_spec_cat_vendor_rating')
        ->fields(array(
          'rating' => $rating['rating'],
        ))
        ->condition('spec_cat_id', $cat_key, '=')
        ->condition('nid', $nid, '=')
        ->execute();
      } else {
        $insert_id = db_insert('arr_spec_cat_vendor_rating')
        ->fields(array(
          'spec_cat_id' => $cat_key,
          'nid' => $nid,
          'rating' => $rating['rating'],
        ))
        ->execute();
      }
      
      foreach ($spec_data as $spec_id => $vals) {
        $fields = array(
          'spec_id' => $spec_id,
          'nid' => $nid,
          'has_spec' => !empty($vals['has_spec']) ? 1 : 0,
          'spec_comment' => $vals['spec_comment'],
        );
        
        $v_spec_id = db_insert('arr_spec_vendors')
        ->fields($fields)
        ->execute();        
      }
    }
  }
  
  drupal_set_message('Technical specifications saved successfully.');
}