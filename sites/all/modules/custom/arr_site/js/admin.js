(function($) {
  $(document).ready(function() {
    var a_class = 'active-cat';
    var $tid = $('.vendor-tid-select');
    
    $tid.bind('change', function() {
      var $t = $(this);
      var val = $t.val();
      var $cats = $('#cat-wrapper-'+val);
      var cats_html = $cats.html();
      
      $('.spec-cat-select').removeClass(a_class);
      
      if (typeof cats_html == 'string') {
        $cats.addClass(a_class);
      }
    });
  });
})(jQuery);