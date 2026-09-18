| Component          | Responsibility         | Input          | Output           | Primary owner | First evidence   |
|--------------------|------------------------|----------------|------------------|---------------|------------------|
| CameraInput        | Provide valid frames   | device / video | cv::Mat          | Jonathan      | saved frame      |
| MarkerDetector     | Find marker candidates | cv::Mat        | corners          | Jonathan      | debug overlay    |
| InteractionMapping | Map state to control   | MarkerState    | double / command | Kalle         | known-value test |
| OutputAdapter      | Send to application    | control value  | OSC / app action | Kalle         | received value   |