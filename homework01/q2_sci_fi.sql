SELECT primary_title, premiered, runtime_minutes || ' (mins)'
FROM titles
WHERE genres LIKE '%Sci-Fi%'
ORDER BY runtime_minutes DESC
LIMIT 10;
