<?php
    $save_file_path = "frame_ring/frame_";
    $settings_file_path = "frame_ring/ring_buffer_settings.json";
    $commands_filename = "commands.json";
    $logs_filename = "logs.txt";

    $resp_arr = "";
    $frame_ring_buffer_size = 40;

    if(isset($_POST["code"]) &&  $_POST["code"] == "cmd_set"){
        $commands_file = fopen($commands_filename, "r");
        $command_arr = json_decode(fread($commands_file, filesize($commands_filename)), true);
        fclose($commands_file);

        if(!$command_arr){
            $command_arr = array("cmd" => $_POST["cmd"], "speed" => $_POST["speed"], "degree" => $_POST["degree"], "duration" => $_POST["duration"]);
            echo  "Command file content created";
        }else if($command_arr["cmd"] == ""){
            $command_arr["cmd"] = $_POST["cmd"];
            $command_arr["speed"] = $_POST["speed"];
            $command_arr["degree"] = $_POST["degree"];
            $command_arr["duration"] = $_POST["duration"];

            $commands_file = fopen($commands_filename, "w");
            if(fwrite($commands_file, json_encode($command_arr))){
                echo  "Command '".$_POST["cmd"]."' sent!";
            }
            fclose($commands_file);
        }else if(isset($_POST["cmd"]) && $_POST["cmd"] == "stop"){
            $command_arr["cmd"] = $_POST["cmd"];
            $command_arr["speed"] = "";
            $command_arr["degree"] = "";
            $command_arr["duration"] = "";

            $commands_file = fopen($commands_filename, "w");
            if(fwrite($commands_file, json_encode($command_arr))){
                echo  "'STOP' command sent!";
            }
            fclose($commands_file);
        }else{
            echo "ERR:Can not send another command! Current command not read by lunochod!";
        }
    }else if(isset($_POST["code"]) &&  $_POST["code"] == "cmd_get"){
        $commands_file = fopen($commands_filename, "r");
        $command_arr = json_decode(fread($commands_file, filesize($commands_filename)), true);
        fclose($commands_file);

        if(!$command_arr){
            echo "No command file!";
        }else if(isset($command_arr) && $command_arr["cmd"] != ""){
            $response = "cmd=".$command_arr["cmd"]."&speed=".$command_arr["speed"]."&degree=".$command_arr["degree"]."&duration=".$command_arr["duration"];
            echo $response;
            $command_arr["cmd"] = "";
            $command_arr["speed"] = "";
            $command_arr["degree"] = "";
            $command_arr["duration"] = "";

            $commands_file = fopen($commands_filename, "w");
            fwrite($commands_file, json_encode($command_arr));
            fclose($commands_file);

            if(isset($_POST["log"])){
                $log_file = fopen($logs_filename, "a");
                fwrite($log_file, $_POST["log"]);
                fclose($log_file);
            }
        }else{
            echo "No command sent...";
        }
    }else if(isset($_POST["code"]) &&  $_POST["code"] == "get_logs"){
        $log_file = fopen($logs_filename, "r");
        if(filesize($logs_filename)){
            $resp_str = fread($log_file, filesize($logs_filename));
            fclose($log_file);
            echo $resp_str;
        }else{
            $log_file = fopen($logs_filename, "w");
            fclose($log_file);
            echo "No logs";
        }
    }else if(isset($_POST["code"]) &&  $_POST["code"] == "get_filename"){
        $settings_file = fopen($settings_file_path, "r");
        $settings = json_decode(fread($settings_file, filesize($settings_file_path)), true);
        fclose($settings_file);

        echo $save_file_path.$settings["curr_r_index"].".bmp";
        if($settings["curr_r_index"] > $settings["curr_w_index"]){
            if($settings["curr_r_index"] == ($settings["size"] - 1)){
                if($settings["curr_w_index"] > 1){
                    $settings["curr_r_index"] = 1;
                }
            }else{
                $settings["curr_r_index"]++;
            }
        }else if($settings["curr_r_index"] + 2 < $settings["curr_w_index"]){
            $settings["curr_r_index"]++;
        }

        $settings_file = fopen($settings_file_path, "w");
        fwrite($settings_file, json_encode($settings));
        fclose($settings_file);
    }else{
        foreach($_FILES as $file){
            $name = basename($file['name']);
            $tmp_name = $file['tmp_name'];
            $size = $file['size'];

            // GET INDEX FOR FILE WRITE
            $settings_file = fopen($settings_file_path, "r");
            $settings = json_decode(fread($settings_file, filesize($settings_file_path)), true);
            fclose($settings_file);

            // SET NEW FILE PATH
            $save_file_path .= $settings["curr_w_index"].".bmp";

            // INCREMENT WRITE INDEX
            if($settings["curr_w_index"] == ($frame_ring_buffer_size - 1))
                $settings["curr_w_index"] = 0;
            else
                $settings["curr_w_index"]++;
            $settings["size"] = $frame_ring_buffer_size;

            // UPDATE SETTINGS AND SAVE THEM INTO FILE
            $settings_file = fopen($settings_file_path, "w");
            fwrite($settings_file, json_encode($settings));
            fclose($settings_file);

            // SAVE FRAME INFO INTO .BMP FILE
            $rec_file = fopen($tmp_name, "r");
            $bmp_file = fopen($save_file_path, "w");
            $rec_info = fread($rec_file, filesize($tmp_name));

            // FORMULATE AND SEND RESPONSE
            $resp_arr = "";
            if(fwrite($bmp_file, $rec_info)){
                $resp_arr =  "BMP file saved successfully ('".$save_file_path."')";
            }else{
                $resp_arr =  "ERR:BMP file not saved!";
            }

            // CLOSE FILES
            fclose($bmp_file);
            fclose($rec_file);
    
            // RETURN RESPONSE TO ESP32
            echo $resp_arr;
        }
    }
?>