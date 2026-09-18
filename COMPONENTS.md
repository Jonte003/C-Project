+--------------------+------------------------+----------------+------------------+---------------+------------------+
| Component          | Responsibility         | Input          | Output           | Primary owner | First evidence   |
+--------------------+------------------------+----------------+------------------+---------------+------------------+
| CameraInput        | Provide valid frames   | device / video | cv::Mat          | Name          | saved frame      |
+--------------------+------------------------+----------------+------------------+---------------+------------------+
| MarkerDetector     | Find marker candidates | cv::Mat        | corners          | Name          | debug overlay    |
+--------------------+------------------------+----------------+------------------+---------------+------------------+
| InteractionMapping | Map state to control   | MarkerState    | double / command | Name          | known-value test |
+--------------------+------------------------+----------------+------------------+---------------+------------------+
| OutputAdapter      | Send to application    | control value  | OSC / app action | Name          | received value   |
+--------------------+------------------------+----------------+------------------+---------------+------------------+