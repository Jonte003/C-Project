| Component          | Responsibility         | Input          | Output           | Primary owner | First evidence   |
|--------------------|------------------------|----------------|------------------|---------------|------------------|
| CameraInput        | Provide valid frames   | device / video | cv::Mat          | Jonathan      | video output     |
| MarkerDetector     | Find and read markers  | cv::Mat        | corners / ID     | Jonathan      | debug overlay    |
| InteractionMapping | Map state to control   | MarkerState    | double / command | Kalle         | known-value test |
| OutputAdapter      | Send to application    | control value  | OSC / app action | Kalle         | received value   |