SELECT DISTINCT(p.person_id), p.name
FROM people AS p
JOIN crew AS c ON c.person_id=p.person_id
WHERE c.title_id IN (
    SELECT c.title_id
    FROM crew AS c
    JOIN people AS p ON p.person_id=c.person_id
    WHERE p.name='Nicole Kidman' AND p.born=1967
) AND c.category IN ('actory', 'actress');
