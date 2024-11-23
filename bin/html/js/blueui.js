function CheckSearchForm(searchForm) {
	if (searchForm.search.value=="") {
		alert("Please enter at least one keyword to search");
		searchForm.search.focus();
		return false;
	}
	return true
}
function mOver(menuItem) {
   menuItem.style.cursor = 'hand';
   menuItem.style.color = '#ffd275';
}
function mOut(menuItem) {
   menuItem.style.cursor = 'default';
   menuItem.style.color = '#ffffff';
}
function mOverMenu(menuItem){
   menuItem.style.cursor = 'hand';
	menuItem.style.borderColor = "#9d9d9d";
	menuItem.style.backgroundColor = "#f1f1f1";
	menuItem.style.color = "#000000";
}
function mOutMenu(menuItem){
   menuItem.style.cursor = 'default';
	menuItem.style.borderColor = "#ffffff";
	menuItem.style.backgroundColor = "#ffffff";
	menuItem.style.color = "#000000";
}
function mDown(url) {
	window.location = url;
}

function newWindow(page) {
	newWindow = window.open(page, 'NewWindow', 'toolbar=yes,menubar=yes,location=yes,scrollbars=yes,resizable=yes') ;
}

function tabOver(tab) {
	if (document.getElementById) // DOM3 = IE5, NS6
	{
   		document.getElementById(tab).className = 'tabLINKhover';
   	}
}
function tabOut(tab) {
	if (document.getElementById) // DOM3 = IE5, NS6
	{
   		document.getElementById(tab).className = 'tabLINK';
   	}
}
function toggleVisibleMenu(array, id)
{
  for ( var x = 0; x < array.length; ++x )  
  {
    var e = document.getElementById(array[x]) ;
    e.style.display = (array[x] == id) ? "" : "none" ;
  }
}
function setColor(id,over) {
  var e = document.getElementById(id) ;
  e.style.color = over? '#ff9900': '#333333' ;
}

function GetDay(iDay) {
	var DayArray = new Array("Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday") ;
	return DayArray[iDay] ;
}

function GetMonth(iMonth) {
	var arMonth = new Array("January", "February", "March", "April", "May", "June","July", "August", "September", "October", "November", "December") ;
	return arMonth[iMonth] ;
}

function GetYear(year) {
	if (year < 2000) {
		year = year + 1900;
	}
	return year;
}

function getDateStrWithDOW() {
	var today = new Date() ;
	var mon = GetMonth(today.getMonth()) ;
	var day = GetDay(today.getDay()) ;
	var year = GetYear(today.getYear()) ;
	var hours = today.getHours() ;
	return ('<font color=\"#ffffff\">' + day + ', ' + mon + ' ' + today.getDate() + ', ' + year + '</font>') ;
}
