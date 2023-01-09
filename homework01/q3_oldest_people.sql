SELECT name, IFNULL(died, strftime('%Y', DATE()))-born AS age
FROM people WHERE born>=1900 AND died>=born
ORDER BY age DESC, name
LIMIT 20;
