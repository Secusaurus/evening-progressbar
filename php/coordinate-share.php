<?php

// use external credentials
$credentials = json_decode(file_get_contents('restricted/credentials.json'), true);


/**
 * Get distance between two GPS coordinates
 * @param Object $coord1 First coordinate (latitude, longitude)
 * @param Object $coord2 Second coordinate (latitude, longitude)
 * @return float Distance in kilometers
 */
function haversine($coord1, $coord2)
{
    $earth_radius = 6371;
    $lat1 = $coord1['latitude'];
    $lon1 = $coord1['longitude'];
    $lat2 = $coord2['latitude'];
    $lon2 = $coord2['longitude'];

    $delta_lat = deg2rad($lat2 - $lat1);
    $delta_lon = deg2rad($lon2 - $lon1);

    $a = sin($delta_lat / 2) * sin($delta_lat / 2) +
         cos(deg2rad($lat1)) * cos(deg2rad($lat2)) *
         sin($delta_lon / 2) * sin($delta_lon / 2);

    $c = 2 * atan2(sqrt($a), sqrt(1 - $a));

    return $earth_radius * $c;
}


// POST-Request: Store GPS coordinate
if ($_SERVER['REQUEST_METHOD'] === 'POST')
{
    if (!isset($_GET['token']) ||
        $_GET['token'] !== $credentials["post_token"])
    {
        file_put_contents('restricted/log.txt', 'Unauthorized: ' . $_GET['token']);
            header('HTTP/1.0 401 Unauthorized');
            echo "ERROR: Unauthorized";
            exit;
    }

    if (isset($_GET['lat'], $_GET['lon']))
    {
        $gps_lat = floatval($_GET['lat']);
        $gps_lon = floatval($_GET['lon']);
       
        $data = [
            'latitude' => $gps_lat,
            'longitude' => $gps_lon,
            'timestamp' => time(),
        ];
        file_put_contents('restricted/log.txt', 'Saved');

        file_put_contents('restricted/coordinates.json', json_encode($data));

        echo "Saved coordinates: " . $gps_lat . " , " . $gps_lon . " at " . date('Y-m-d H:i:s', $data['timestamp']);
        exit;
    }
    else
    {
        file_put_contents('restricted/log.txt', 'No coordinates');
        echo "ERROR: No coordinates provided.";
        exit;
    }
}


// GET-Request: Get distance to reference point
if ($_SERVER['REQUEST_METHOD'] === 'GET')
{
    if (!isset($_GET['token']) ||
        $_GET['token'] !== $credentials["get_token"])
    {
        header('HTTP/1.0 401 Unauthorized');
        echo "-1\nERROR: Unauthorized";
        exit;
    }

    if (file_exists('restricted/coordinates.json'))
    {
        $data = json_decode(file_get_contents('restricted/coordinates.json'), true);
        $current_time = time();
        $time_diff = $current_time - $data['timestamp'];

        // Only return distance if the data is not older than 2 hours (7200 seconds)
        if ($time_diff > 7200)
        {
            echo "-1\nERROR: Last update is too old.";
            exit;
        }
        else
        {
            // Return distance in km
            $distance = haversine($data, $credentials["target_coordinates"]);
            echo number_format($distance, 2);
            exit;
        }
    } else {
        echo "-1\nERROR: No saved coordinates.";
        exit;
    }
}
?>