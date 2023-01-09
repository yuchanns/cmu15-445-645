SELECT p.name, count(1) AS NUM_APPEARANCES
FROM crew AS c JOIN people AS p ON c.person_id=p.person_id
GROUP BY c.person_id
ORDER BY NUM_APPEARANCES DESC
LIMIT 20;
