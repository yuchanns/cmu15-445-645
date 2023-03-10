SELECT
CAST(ROUND(t.premiered/10-0.5)*10 AS INT) || 's' AS DECADE,
ROUND(AVG(r.rating), 2) AS AVG_RATING,
MAX(r.rating) AS TOP_RATING,
MIN(r.rating) AS MIN_RATING,
COUNT(1) AS NUM_RELEASES
FROM titles AS t JOIN ratings AS r ON r.title_id=t.title_id
WHERE t.premiered IS NOT NULL
GROUP BY DECADE
ORDER BY AVG_RATING DESC, DECADE;
