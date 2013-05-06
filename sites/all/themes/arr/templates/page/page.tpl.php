<?php

/**
 * @file
 * Theme implementation to display a single Drupal page.
 *
 * The doctype, html, head and body tags are not in this template. Instead they
 * can be found in the html.tpl.php template normally located in the
 * modules/system directory.
 *
 * Available variables:
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/bartik.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or
 *   prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 *
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title: The page title, for use in the actual HTML content.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 * - $messages: HTML for status and error messages. Should be displayed
 *   prominently.
 * - $tabs (array): Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links (array): Actions local to the page, such as 'Add menu' on the
 *   menu administration interface.
 * - $feed_icons: A string of all feed icons for the current page.
 * - $node: The node object, if there is an automatically-loaded node
 *   associated with the page, and the node ID is the second argument
 *   in the page's path (e.g. node/12345 and node/12345/revisions, but not
 *   comment/reply/12345).
 *
 * Regions:
 * - $page['header']: Items for the header region.
 * - $page['nav']: Items for the primary navigation.
 * - $page['content_top']: Items to be displayed to the right of the page title.
 * - $page['content']: The main content of the current page.
 * - $page['right_col']: Items for the sidebar.
 * - $page['footer']: Items for the footer region.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see template_process()
 * @see arr_process_page()
 * @see html.tpl.php
 */

?>
<div id="page-wrapper">
  <div id="page-top" class="row container">
    <div class="columns clearfix">
      <?php if ($logo) { ?>
        <div class="column left">
          <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" id="logo">
            <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
          </a>
        </div>
      <?php } ?>
      
      <?php if ($page['header'] || $page['nav']) { ?>
        <div class="column right">
          <header id="head" class="row">
            <?php if ($page['header']) { ?>
              <?php print render($page['header']); ?>
            <?php } ?>
          </header>
          <!-- /#head /.row -->
          
          <nav id="nav" class="row">
            <?php if ($page['nav']) { ?>
              <?php print render($page['nav']); ?>
            <?php } ?>
          </nav>
          <!-- /#nav /.row -->
        </div>
        <!-- /.column /.right -->
      <?php } ?>
    </div>
    <!-- /.columns /.clearfix -->
  </div>
  <!-- /#page-top /.row /.container -->
  
  <?php if ($messages) { ?>
    <div id="messages-wrapper" class="row container">
      <div class="columns">
        <?php print $messages; ?>
      </div>
      <!-- /.columns -->
    </div>
    <!-- /#messages-wrapper /.row /.container -->
  <?php } ?>
  
  <div id="page-main" class="row container">
    
    <div class="columns clearfix">
      <div class="column left"<?php print $schema_main_content; ?>>
        <?php if ((!$is_front && $title) || !empty($description) || $page['content_top']): ?>
          <div id="content-top" class="row clearfix">
            <div class="column left">
              <?php if (!$is_front) { ?>
                <?php if ($breadcrumb): ?>
                  <?php print $breadcrumb; ?>
                <?php endif; ?>
                
                <p class="vendor-top">All Rave Reviews On</p>
                
                <h1 id="page-title" class="title"<?php print $schema_vendor_name; ?>>
                  <?php //if ($is_vendor_page && !empty($vendor_category)) { ?>
                    <?php //print $vendor_category; ?><!-- &ndash; -->
                  <?php //} ?>
                  <?php print $title; ?>
                </h1>
              <?php } ?>
              
              <?php if (!empty($description)) { ?>
                <div id="description-wrapper" class="description reveal"<?php print $schema_vendor_desc; ?>>
                  <?php print $description; ?>
                </div>
                <!-- /#description-wrapper /.description /.reveal -->
                <a class="toggle-description right" href="#description-wrapper"><span>Expand</span> Description</a>
              <?php } ?>
            </div>
            <!-- /.column /.left -->
            
            <?php if ($page['content_top']) { ?>
              <div class="column right">
                <?php print render($page['content_top']); ?>
              </div>
              <!-- /.column /.right -->
            <?php } ?>
          </div>
          <!-- /#content-top /.row /.clearfix -->
        <?php endif; ?>
        
        <div id="content" class="row">          
          <?php print render($page['content']); ?>
        </div>
        <!-- /#content /.row -->
      </div>
      <!-- /.column /.left -->
      
      <?php if ($page['right_col']) { ?>
        <div class="column right">
          <?php print render($page['right_col']); ?>
        </div>
        <!-- /.column /.right -->
      <?php } ?>
    </div>
    <!-- /.columns /.clearfix -->
    
    <?php if ($page['content_bottom']) { ?>
      <div id="content-bottom" class="row container">
        <div class="columns clearfix">
          <?php print render($page['content_bottom']); ?>
        </div>
        <!-- /.coluns /.clearfix -->
      </div>
      <!-- /#content-bottom /.row /.container -->
    <?php } ?>
  </div>
  <!-- /#page-main /.row -->
  
  <?php if ($page['footer_top']) { ?>
    <div id="footer-top" class="row">
      <div class="columns clearfix">
        <?php print render($page['footer_top']); ?>
      </div>
      <!-- /.columns /.clearfix -->
    </div>
    <!-- /#footer-top /.row -->
  <?php } ?>
</div>
<!-- /#page-wrapper -->

<?php if ($page['footer_bottom']) { ?>
  <footer id="page-bottom" class="row container">
    <div class="columns">
      <?php print render($page['footer_bottom']); ?>
    </div>
    <!-- /.columns -->
  </footer>
  <!-- /#page-bottom /.row /.container -->
  
  <?php if ($page['popups']) { ?>
    <div id="popups">
      <?php print render($page['popups']); ?>
    </div>
    <!-- /#popups -->
  <?php } ?>
<?php } ?>
