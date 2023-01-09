SELECT
t.primary_title, r.votes
FROM crew AS c
JOIN ratings AS r ON r.title_id=c.title_id
JOIN titles AS t ON t.title_id=r.title_id
JOIN people AS p ON p.person_id=c.person_id
WHERE p.name LIKE '%Cruise%' AND born=1962
ORDER BY r.votes DESC
LIMIT 10;
