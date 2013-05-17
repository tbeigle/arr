<?php

/**
 * Clones existing vendor type's categories and specs for a new vendor type as new categories and new specs
 */
function arr_site_clone_vendor($mapped_tid, $unmapped_tid) {
  $groups = array();
  
  $sql =
    'SELECT ' .
      'ascm.`spec_cat_id` AS `spec_cat_id`, ' .
      'c.`spec_category` AS `spec_category`, ' .
      's.`spec_id` AS `spec_id`, ' .
      's.`spec` AS `spec`, ' .
      'asm.`weight` AS `spec_weight`, ' .
      'asm.`points_yes` AS `points_yes`, ' .
      'asm.`points_no` AS `points_no`, ' .
      'ascm.`weight` AS `spec_cat_weight` ' .
    'FROM {arr_specs} s ' .
      'LEFT JOIN {arr_spec_map} asm ' .
        'ON s.`spec_id` = asm.`spec_id` ' .
      'LEFT JOIN {arr_spec_cat_map} ascm ' .
        'ON asm.`tcid` = ascm.`tcid` ' .
      'LEFT JOIN {arr_spec_categories} c ' .
        'ON ascm.`spec_cat_id` = c.`spec_cat_id` ' .
    'WHERE ascm.`tid` = :tid ' .
    'ORDER BY ascm.`tid`;';
  $result = db_query($sql, array(':tid' => $mapped_tid));
  
  foreach ($result as $row) {
    if (!isset($groups[$row->spec_cat_id])) {
      $groups[$row->spec_cat_id] = array(
        'arr_spec_categories' => array(
          'spec_category' => $row->spec_category,
        ),
        'arr_spec_cat_map' => array(
          'tid' => $unmapped_tid,
          'spec_cat_id' => 0,// Just a placeholder
          'weight' => $row->spec_cat_weight,
        ),
        'specs' => array(),
      );
    }
    
    $groups[$row->spec_cat_id]['specs'][$row->spec_id] = array(
      'arr_specs' => array(
        'spec' => $row->spec,
      ),
      'arr_spec_map' => array(
        'tcid' => 0,// Just a placeholder
        'spec_id' => 0,// Just a placeholder
        'weight' => $row->spec_weight,
        'points_yes' => $row->points_yes,
        'points_no' => $row->points_no,
      ),
    );
  }
  
  foreach ($groups as $old_spec_cat_id => $spec_cat_data) {
    $spec_category = $spec_cat_data['arr_spec_categories'];
    $spec_cat_map = $spec_cat_data['arr_spec_cat_map'];
    $specs = $spec_cat_data['specs'];
    
    $spec_cat_map['spec_cat_id'] = db_insert('arr_spec_categories')->fields($spec_category)->execute();
    $tcid = db_insert('arr_spec_cat_map')->fields($spec_cat_map)->execute();
    
    foreach ($specs as $spec_data) {
      $spec = $spec_data['arr_specs'];
      $spec_map = $spec_data['arr_spec_map'];
      
      $spec_map['tcid'] = $tcid;
      $spec_map['spec_id'] = db_insert('arr_specs')->fields($spec)->execute();
      $tscid = db_insert('arr_spec_map')->fields($spec_map)->execute();
    }
  }
}

/**
 * Fetch all node tech spec values for a specified taxonomy term
 */
function arr_site_fetch_node_spec_data_by_tid($tid) {
  $data = array(
    'spec_categories' => array(),
    'specs' => array(),
    'nodes' => array(),
  );
  
  $sql =
    'SELECT '.
      'v.`spec_id` AS `spec_id`, ' .
      'v.`nid` AS `nid`, ' .
      'v.`has_spec` AS `has_spec`, ' .
      'v.`spec_comment` AS `spec_comment`, ' .
      'c.`spec_cat_id` AS `spec_cat_id`, ' .
      'r.`rating` AS `cat_rating` ' .
    'FROM {arr_spec_vendors} v ' .
      'LEFT JOIN {arr_specs} s ' .
        'ON v.`spec_id` = s.`spec_id` ' .
      'LEFT JOIN {arr_spec_map} asm ' .
        'ON s.`spec_id` = asm.`spec_id` ' .
      'LEFT JOIN {arr_spec_cat_map} ascm ' .
        'ON asm.`tcid` = ascm.`tcid` ' .
      'LEFT JOIN {arr_spec_categories} c ' .
        'ON ascm.`spec_cat_id` = c.`spec_cat_id` ' .
      'LEFT JOIN {arr_spec_cat_vendor_rating} r ' .
        'ON v.`nid` = r.`nid` ' .
          'AND c.`spec_cat_id` = r.`spec_cat_id` ' .
    'WHERE ascm.`tid` = :tid ' .
    'ORDER BY ascm.`tid`;';
  $result = db_query($sql, array(':tid' => $tid));
  
  foreach ($result as $row) {
    if (!isset($data['nodes'][$row->nid])) {
      $data['nodes'][$row->nid] = array();
    }
    
    if (!isset($data['nodes'][$row->nid][$row->spec_cat_id])) {
      $data['nodes'][$row->nid][$row->spec_cat_id] = array(
        'rating' => $row->cat_rating,
        'specs' => array(),
      );
    }
    
    if (!in_array($row->spec_cat_id, $data['spec_categories'])) {
      $data['spec_categories'][] = $row->spec_cat_id;
    }
    
    if (!in_array($row->spec_id, $data['specs'])) {
      $data['specs'][] = $row->spec_id;
    }
    
    $data['nodes'][$row->nid][$row->spec_cat_id]['specs'][$row->spec_id] = array(
      'has_spec' => $row->has_spec,
      'spec_comment' => $row->spec_comment,
    );
  }
  
  return $data;
}

/**
 * Fetch the tech spec names for all specs for a specified taxonomy term
 */
function arr_site_fetch_tech_specs_by_tid($tid) {
  $groups = array();
  
  $sql =
    'SELECT '.
      's.`spec_id` AS `spec_id`, ' .
      's.`spec` AS `spec`, ' .
      'c.`spec_cat_id` AS `spec_cat_id`, ' .
      'c.`spec_category` AS `spec_category` ' .
    'FROM {arr_specs} s ' .
      'LEFT JOIN {arr_spec_map} asm ' .
        'ON s.`spec_id` = asm.`spec_id` ' .
      'LEFT JOIN {arr_spec_cat_map} ascm ' .
        'ON asm.`tcid` = ascm.`tcid` ' .
      'LEFT JOIN {arr_spec_categories} c ' .
        'ON ascm.`spec_cat_id` = c.`spec_cat_id` ' .
    'WHERE ascm.`tid` = :tid ' .
    'ORDER BY ' .
      'ascm.`tid`, ' .
      'ascm.`weight`, ' .
      'c.`spec_category`, ' .
      'asm.`weight`, ' .
      's.`spec`;';
  $result = db_query($sql, array(':tid' => $tid));
  
  foreach ($result as $row) {
    if (!isset($groups[$row->spec_cat_id])) {
      $groups[$row->spec_cat_id] = array(
        'cat' => $row->spec_category,
        'rows' => array(),
      );
    }
    
    $groups[$row->spec_cat_id]['rows'][$row->spec_id] = $row->spec;
  }
  
  return $groups;
}

/**
 * Fetch a node's tech specs data
 */
function arr_site_fetch_tech_specs_for_node($node) {
  $output = array(
    'specs' => array(),
    'groups' => array(),
  );
  
  if (is_numeric($node)) {
    $nid = $node;
    $node = node_load($nid);
  } elseif (!empty($node->nid)) {
    $nid = $node->nid;
  }
  
  if (empty($nid)) {
    return FALSE;
  }
  
  $tids = array();
  
  if (!empty($node->field_vendor_categories[LANGUAGE_NONE])) {
    foreach ($node->field_vendor_categories[LANGUAGE_NONE] as $key => $data) {
      if (!empty($data['tid'])) {
        $tids[] = $data['tid'];
      }
    }
  }
  
  $output['specs'] = array();
  
  $sql =
    'SELECT ' .
      '`spec_id`, ' .
      '`has_spec`, ' .
      '`spec_comment` ' .
    'FROM {arr_spec_vendors} ' .
    'WHERE `nid` = :nid ' .
    'ORDER BY `nid`;';
  $result = db_query($sql, array(':nid' => $nid));
  
  foreach ($result as $row) {
    $output['specs'][$row->spec_id] = array(
      'has_spec' => $row->has_spec,
      'spec_comment' => $row->spec_comment
    );
  }
  
  $output['groups'] = array();
  
  $sql =
    'SELECT '.
      's.`spec_id` AS `spec_id`, ' .
      's.`spec` AS `spec`, '.
      'c.`spec_cat_id` AS `spec_cat_id`, ' .
      'c.`spec_category` AS `spec_category`, ' .
      'ascm.`tid` AS `tid`, ' .
      't.`name` AS `ven_cat` ' .
    'FROM {arr_specs} s ' .
      'LEFT JOIN {arr_spec_map} asm ' .
        'ON s.`spec_id` = asm.`spec_id` ' .
      'LEFT JOIN {arr_spec_cat_map} ascm ' .
        'ON asm.`tcid` = ascm.`tcid` ' .
      'LEFT JOIN {arr_spec_categories} c ' .
        'ON ascm.`spec_cat_id` = c.`spec_cat_id` ' .
      'LEFT JOIN {taxonomy_term_data} t ' .
        'ON ascm.`tid` = t.`tid` ' .
    'WHERE ascm.`tid` IN (' . implode(',', $tids) . ') ' .
    'ORDER BY ' .
      't.`weight`, ' .
      't.`name`, ' .
      'ascm.`weight`, ' .
      'c.`spec_category`, ' .
      'asm.`weight`, ' .
      's.`spec`;';
  $result = db_query($sql);
  
  foreach ($result as $row) {
    if (!isset($output['groups'][$row->tid])) {
      $output['groups'][$row->tid] = array(
        'term' => $row->ven_cat,
        'rows' => array(),
      );
    }
    
    if (!isset($output['groups'][$row->tid]['rows'][$row->spec_cat_id])) {
      $output['groups'][$row->tid]['rows'][$row->spec_cat_id] = array(
        'cat' => $row->spec_category,
        'rows' => array(),
      );
    }
    
    $output['groups'][$row->tid]['rows'][$row->spec_cat_id]['rows'][$row->spec_id] = $row->spec;
  }
  
  return $output;
}

/**
 * Function to fetch an individual tech spec record
 */
function arr_site_get_tech_spec($tid, $id = 0, $all = FALSE) {
  if ($all) {
    $output = array();
    
    $sql =
      'SELECT ' .
        's.`spec_id` AS `spec_id`, ' .
        's.`spec` AS `spec`, ' .
        'asm.`weight` AS `weight`, ' .
        'asm.`points_yes` AS `points_yes`, ' .
        'asm.`points_no` AS `points_no`, ' .
        'ascm.`spec_cat_id` as `spec_cat_id`, ' .
      'FROM {arr_specs} s '.
        'LEFT JOIN {arr_spec_map} asm ' .
          'ON s.`spec_id` = asm.`spec_id` ' .
        'LEFT JOIN {arr_spec_cat_map} ascm ' .
          'ON asm.`tcid` = ascm.`tcid` ' .
      'WHERE ascm.`tid` = :tid ' .
      'ORDER BY s.`spec`;';
    $result = db_query($sql, array(':tid' => $tid));
    
    foreach ($result as $row) {
      $output[] = $row;
    }
  } elseif (!empty($id)) {
    $sql =
      'SELECT ' .
        's.`spec_id` AS `spec_id`, ' .
        's.`spec` AS `spec`, ' .
        'asm.`weight` AS `weight`, ' .
        'asm.`points_yes` AS `points_yes`, ' .
        'asm.`points_no` AS `points_no`, ' .
        'ascm.`spec_cat_id` as `spec_cat_id` ' .
      'FROM {arr_specs} s '.
        'LEFT JOIN {arr_spec_map} asm ' .
          'ON s.`spec_id` = asm.`spec_id` ' .
        'LEFT JOIN {arr_spec_cat_map} ascm ' .
          'ON asm.`tcid` = ascm.`tcid` ' .
      'WHERE ascm.`tid` = :tid ' .
        'AND s.`spec_id` = :spec_id ' .
      'ORDER BY `spec_id` LIMIT 1;';
    $output = db_query($sql, array(':tid' => $tid, ':spec_id' => $id))->fetchObject();
  } else {
    $output = (object) array(
      'spec_id' => '',
      'spec_cat_id' => '',
      'spec' => '',
      'points_yes' => 1,
      'points_no' => 0,
      'weight' => 0,
    );
  }
  
  return $output;
}

/**
 * Checks whether this is a category page with vendors
 */
function arr_site_is_category_page($vid = 0) {
  if (empty($vid)) {
    $vid = variable_get('arr_site_vendor_vocabulary', 0);
  }
  
  if (arg(0) == 'taxonomy' && arg(1) == 'term') {
    if ($tid = arg(2)) {
      $sql =
        'SELECT COUNT(*) '.
        'FROM {taxonomy_term_data} '.
        'WHERE `tid` = :tid '.
          'AND `vid` = :vid '.
        'ORDER BY `tid`, `vid`;';
      $count = db_query($sql, array(':tid' => $tid, ':vid' => $vid))->fetchField();
    }
  }
  
  return !empty($count) ? $tid : 0;
}

/**
 * Links new vendor term to existing tech spec categories and specs
 */
function arr_site_map_new_vendor($mapped_tid, $unmapped_tid) {
  $sql =
    'SELECT ' .
      'asm.`spec_id` AS `spec_id`, ' .
      'asm.`weight` AS `spec_weight`, ' .
      'asm.`points_yes` AS `points_yes`, ' .
      'asm.`points_no` AS `points_no`, ' .
      'ascm.`spec_cat_id` AS `spec_cat_id`, ' .
      'ascm.`weight` AS `spec_cat_weight` ' .
    'FROM {arr_spec_map} asm ' .
      'LEFT JOIN {arr_spec_cat_map} ascm ' .
        'ON asm.`tcid` = ascm.`tcid` ' .
    'WHERE ascm.`tid` = :tid ' .
    'ORDER BY ascm.`tid`;';
  $result = db_query($sql, array(':tid' => $mapped_tid));
  
  foreach ($result as $row) {
    if (!isset($groups[$row->spec_cat_id])) {
      $groups[$row->spec_cat_id] = array(
        'tid' => $unmapped_tid,
        'spec_cat_id' => $row->spec_cat_id,
        'weight' => $row->spec_cat_weight,
        'specs' => array(),
      );
    }
    
    $groups[$row->spec_cat_id]['specs'][$row->spec_id] = array(
      'tcid' => 0,// Just a placeholder
      'spec_id' => $row->spec_id,
      'weight' => $row->spec_weight,
      'points_yes' => $row->points_yes,
      'points_no' => $row->points_no,
    );
  }
  
  foreach ($groups as $spec_cat_id => $cat_data) {
    // Check whether the vendor already has this category to avoid duplication
    $sql =
      'SELECT `tcid` ' .
      'FROM {arr_spec_cat_map} ' .
      'WHERE `tid` = :tid ' .
        'AND `spec_cat_id` = :spec_cat_id ' .
      'ORDER BY `tid`, `spec_cat_id`;';
    $tcid = db_query($sql, array(':tid' => $unmapped_tid, ':spec_cat_id' => $spec_cat_id))->fetchField();
    
    $specs = array_pop($cat_data);
    
    if (empty($tcid)) {
      $tcid = db_insert('arr_spec_cat_map')
        ->fields($cat_data)
        ->execute();
    }
    
    foreach ($specs as $spec_id => $spec_data) {
      // Check whether the vendor already has this spec for this category to avoid duplication
      $sql =
        'SELECT `tcsid` ' .
        'FROM {arr_spec_map} ' .
        'WHERE `tcid` = :tcid ' .
          'AND `spec_id` = :spec_id ' .
        'ORDER BY `tcid`, `spec_id`;';
      $tcsid = db_query($sql, array(':tcid' => $tcid, ':spec_id' => $spec_id))->fetchField();
      
      if (empty($tcsid)) {
        $spec_data['tcid'] = $tcid;
        
        $tscid = db_insert('arr_spec_map')
          ->fields($spec_data)
          ->execute();
      }
    }
  }
}

/**
 * Build an options list for vendor categories on a webform
 */
function arr_webform_pre_build_list_vendor_categories() {
  return arr_site_build_list_vendor_categories();
}

/**
 * Build the options list for vendor categories
 */
function arr_site_build_list_vendor_categories($not_tid = 0) {
  global $_arr_site;
  
  $vid = !empty($_arr_site['vendors_vocab']) ? $_arr_site['vendors_vocab'] : 0;
  $output = array();
  
  $sql =
    'SELECT `tid`, `name` '.
    'FROM {taxonomy_term_data} '.
    'WHERE `vid` = :vid '.
      'AND `tid` <> :tid '.
    'ORDER BY `vid`, `name`;';
  $result = db_query($sql, array(':vid' => $vid, ':tid' => $not_tid));
  
  foreach ($result as $row) {
    $output[$row->tid] = t('!name', array('!name' => $row->name));
  }
  
  return $output;
}

/**
 * Build an options list to be used with webforms select list based in drupal roles
 */
function arr_webform_pre_build_list_vendors() {
  global $_arr_site;
  
  $type = !empty($_arr_site['vendor_type']) ? $_arr_site['vendor_type'] : '';
  $contentarray = array();
  
  $sql =
    'SELECT `nid`, `title` '.
    'FROM {node} '.
    'WHERE `type` = :type '.
    'ORDER BY `type`, `title`;';
  $results = db_query($sql, array(':type' => $type));
  
  foreach ($results as $row) {
    $contentarray[$row->nid] = t($row->title);
  }
  
  $contentarray['other'] = 'Other';
  
  return $contentarray;
}
