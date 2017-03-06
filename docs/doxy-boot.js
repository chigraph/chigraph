$( document ).ready(function() {

    $("table").addClass("mdl-data-table mdl-js-data-table");
    $("tr > td").addClass("mdl-data-table__cell--non-numeric");
    $("td.arrow").addClass("mdl-button mdl-js-button mdl-button--fab mdl-js-ripple-effect");

    $(".mlabel").addClass("mdl-chip").removeClass("mlabel").wrapInner('<span class="mdl-chip__text"></span>');
});
