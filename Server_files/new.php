<?php
    $system_file_path = "system.json";

    if($_POST["code"] == "get_cookie"){
        if($_SERVER["REQUEST_METHOD"] == "POST") {
            $info = esp_input($_POST["some_param"]);

            $info_file = fopen("new_info.txt", "a+");
            fwrite($info_file, "\n".$info);
            fclose($info_file);

            $system_file = fopen($system_file_path, "r");
            $system_settings = json_decode(fread($system_file, filesize($system_file_path)));
            fclose($system_file);

            echo $system_settings->key."\0";
        }
    }else if($_POST["code"] == "set_cookie"){
        setcookie("key_key", $_POST["key_key"]);
    }

    function esp_input($data) {
        $data = trim($data);
        $data = stripslashes($data);
        $data = htmlspecialchars($data);
        return $data;
    }
?>
