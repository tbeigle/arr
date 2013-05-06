<?php

/**
 * @file
 * Bartik's theme implementation to display a node.
 *
 * Available variables:
 * - $title: the (sanitized) title of the node.
 * - $content: An array of node items. Use render($content) to print them all,
 *   or print a subset such as render($content['field_example']). Use
 *   hide($content['field_example']) to temporarily suppress the printing of a
 *   given element.
 * - $user_picture: The node author's picture from user-picture.tpl.php.
 * - $date: Formatted creation date. Preprocess functions can reformat it by
 *   calling format_date() with the desired parameters on the $created variable.
 * - $name: Themed username of node author output from theme_username().
 * - $node_url: Direct url of the current node.
 * - $display_submitted: Whether submission information should be displayed.
 * - $submitted: Submission information created from $name and $date during
 *   template_preprocess_node().
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It can be manipulated through the variable $classes_array from
 *   preprocess functions. The default values can be one or more of the
 *   following:
 *   - node: The current template type, i.e., "theming hook".
 *   - node-[type]: The current node type. For example, if the node is a
 *     "Blog entry" it would result in "node-blog". Note that the machine
 *     name will often be in a short form of the human readable label.
 *   - node-teaser: Nodes in teaser form.
 *   - node-preview: Nodes in preview mode.
 *   The following are controlled through the node publishing options.
 *   - node-promoted: Nodes promoted to the front page.
 *   - node-sticky: Nodes ordered above other non-sticky nodes in teaser
 *     listings.
 *   - node-unpublished: Unpublished nodes visible only to administrators.
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 *
 * Other variables:
 * - $node: Full node object. Contains data that may not be safe.
 * - $type: Node type, i.e. story, page, blog, etc.
 * - $comment_count: Number of comments attached to the node.
 * - $uid: User ID of the node author.
 * - $created: Time the node was published formatted in Unix timestamp.
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   teaser listings.
 * - $id: Position of the node. Increments each time it's output.
 *
 * Node status variables:
 * - $view_mode: View mode, e.g. 'full', 'teaser'...
 * - $teaser: Flag for the teaser state (shortcut for $view_mode == 'teaser').
 * - $page: Flag for the full page state.
 * - $promote: Flag for front page promotion state.
 * - $sticky: Flags for sticky post setting.
 * - $status: Flag for published status.
 * - $comment: State of comment settings for the node.
 * - $readmore: Flags true if the teaser content of the node cannot hold the
 *   main body content.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 *
 * Field variables: for each field instance attached to the node a corresponding
 * variable is defined, e.g. $node->body becomes $body. When needing to access
 * a field's raw values, developers/themers are strongly encouraged to use these
 * variables. Otherwise they will have to explicitly specify the desired field
 * language, e.g. $node->body['en'], thus overriding any language negotiation
 * rule that was previously applied.
 *
 * @see template_preprocess()
 * @see template_preprocess_node()
 * @see template_process()
 */
?>

<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?>"<?php print $attributes; ?><?php print $schema_node; ?>>
  <?php if (!empty($node_edit_link)): ?>
    <?php print $node_edit_link; ?>
  <?php endif; ?>
  
  <?php if ($page): ?>
    <div class="<?php print $vendor_top_classes['wrapper']; ?>">
      <?php if (!empty($content['field_logo'])): ?>
        <div class="vendor-logo column left img-wrapper">
          <?php if ($content['field_vendor_website']): ?>
            <a<?php print render($content['field_vendor_website']); ?>>
              <?php print render($content['field_logo']); ?>
            </a>
          <?php else: ?>
            <?php print render($content['field_logo']); ?>
          <?php endif; ?>
        </div>
        <!-- /.vendor-logo /.left -->
      <?php endif; ?>
      
      <?php if ($content['field_arr_rating'] || $content['field_user_rating']): ?>
        <div class="star-ratings column left">
          <div class="stars-wrapper">
            <?php if ($content['field_arr_rating']): ?>
              <?php print render($content['field_arr_rating']); ?>
            <?php endif; ?>
            
            <?php if ($content['field_user_rating']): ?>
              <?php print render($content['field_user_rating']); ?>
            <?php endif; ?>
          </div>
          <!-- /.stars -->
        </div>
        <!-- /.star-ratings -->
      <?php endif; ?>
      
      <?php if ($content['field_vendor_website']): ?>
        <div class="contact-vendor column left">
          <a class="contact-vendor-button"<?php print render($content['field_vendor_website']); ?>>Contact Vendor</a>
        </div>
        <!-- /.contact-vendor /.column /.left -->
      <?php endif; ?>
      
      <?php if ($content['field_lowest_cost']): ?>
        <?php print render($content['field_lowest_cost']); ?>
      <?php endif; ?>
      
      <div id="buttons-vendor-top">
        <ul>
          <li><a class="scrollto" href="#comments">User Reviews</a></li>
          <li><a class="scrollto" href="#comment-form-title">Review Vendor</a></li>
          <li><a class="popup-link" data-popup="#popups .suggest-a-vendor" href="/suggest-vendor">Suggest Vendor</a></li>
        </ul>
      </div>
      <!-- /#buttons-vendor-top -->
    </div>
    <!-- /.row /.clearfix /.vendor-top -->
  <?php endif; ?>
  
  <?php print render($title_prefix); ?>
  <?php if (!$page): ?>
    <?php if (!$is_categories_page): ?>
      <h2<?php print $title_attributes; ?>>
        <a href="<?php print $node_url; ?>"><?php print $title; ?></a>
      </h2>
    <?php else: ?>
      <div class="row vendors">
        <?php if (!empty($content['field_logo'])): ?>
          <a href="<?php print $node_url; ?>" title="<?php print $title; ?>">
            <?php print render($content['field_logo']); ?>
          </a>
        <?php else: ?>
          <h2<?php print $title_attributes; ?>>
            <a href="<?php print $node_url; ?>"><?php print $title; ?></a>
          </h2>              
        <?php endif; ?>
      </div>
      <!-- /.row /.vendors -->
    <?php endif; ?>
  <?php endif; ?>
  <?php print render($title_suffix); ?>
  
  <?php if ($is_categories_page): ?>
    <div class="bottom-rows-wrapper">
      <div class="row arr-rating">
        <?php if ($content['field_arr_rating']): ?>
          <div class="star-ratings column left">
            <div class="stars-wrapper">
              <?php print render($content['field_arr_rating']); ?>
            </div>
            <!-- /.stars -->
          </div>
          <!-- /.star-ratings -->
        <?php endif; ?>
      </div>
      <!-- /.row /.arr-rating -->
      
      <div class="row user-rating">
        <?php if ($content['field_user_rating']): ?>
          <div class="star-ratings column left">
            <div class="stars-wrapper">
              <?php print render($content['field_user_rating']); ?>
            </div>
            <!-- /.stars-wrapper -->
          </div>
          <!-- /.star-ratings -->
        <?php endif; ?>
      </div>
      <!-- /.row /.user-rating -->
      
      <div class="row company-info">
        <?php if ($content['field_vendor_website']): ?>
          <p class="request-a-quote-button">
            <a<?php print render($content['field_vendor_website']); ?>>
              Request a Quote
            </a>
          <p>
        <?php endif; ?>
        
        <?php if (!empty($content['vendor_phone_number'])): ?>
          <p class="vendor-phone">
            <?php print $content['vendor_phone_number']; ?>
          </p>
        <?php endif; ?>
        
        <p class="full-vendor-review">
          <a href="<?php print $node_url; ?>">Full Vendor Review</a>
        </p>
      </div>
      <!-- /.row /.company-info -->
      
      <div class="row bottom-line collapsible expanded">
        <?php if (!empty($content['field_bottom_line'])): ?>
          <?php print render($content['field_bottom_line']); ?>
        <?php endif; ?>
      </div>
      <!-- /.row /.bottom-line -->
    </div>
    <!-- /.bottom-rows-wrapper -->
  <?php endif; ?>
  
  <?php if ($display_submitted): ?>
    <div class="meta submitted">
      <?php print $user_picture; ?>
      <?php print $submitted; ?>
    </div>
  <?php endif; ?>
  
  <?php
    // We hide the comments and links now so that we can render them later.
    hide($content['comments']);
    hide($content['links']);
    hide($content['field_arr_review']);
    
    // Hide the vendor variables we're showing in different locations
    if (!empty($hide_fields)) {
      if (is_array($hide_fields)) {
        foreach ($hide_fields as $hf) {
          if (isset($content[$hf])) hide($content[$hf]);
        }
      }
    }
  ?>
  
  <?php if (!$is_categories_page): ?>
    <div class="content clearfix"<?php print $content_attributes; ?>>
      <div class="schema-review-wrapper"<?php print $schema_review; ?>>
        <?php print render($content); ?>
        
        <?php if (!empty($tech_specs) && $page): ?>
          <?php print render($tech_specs); ?>
        <?php endif; ?>
        
        <?php print render($content['field_arr_review']); ?>
      </div>
    </div>
  <?php endif; ?>

  <?php
    // Remove the "Add new comment" link on the teaser page or if the comment
    // form is being displayed on the same page.
    if ($teaser || !empty($content['comments']['comment_form'])) {
      unset($content['links']['comment']['#links']['comment-add']);
    }
    // Only display the wrapper div if there are links.
    $links = render($content['links']);
    if ($links && !$is_categories_page):
  ?>
    <div class="link-wrapper">
      <?php print $links; ?>
    </div>
  <?php endif; ?>

  <?php print render($content['comments']); ?>

</div>
