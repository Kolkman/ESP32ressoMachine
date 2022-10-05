function c(l) {
    document.getElementById('s').value = l.innerText || l.textContent;
    document.getElementById('p').focus();
    document.getElementById('s1').value = l.innerText || l.textContent;
    document.getElementById('p1').focus();
    document.getElementById('timezone').value = timezone.name();
}