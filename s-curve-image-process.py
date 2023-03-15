# Artık detect.py yok bu kodu çalıştıracaz
import torch, cv2, os, time
from collections import deque, Counter
import serial

ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=1)

model = torch.hub.load(
    os.getcwd(), "custom", source="local", path="best.pt", force_reload=True
)

labels = model.names
models = {0: "stopsign_1", 1: "speed5sign_5", 2: "speed10sign_11", 3: "speed20sign_20"}
limits = {1: "STOP", 5: "5cm/s", 11: "10cm/s", 20: "20cm/s"}
predictions = deque([], maxlen=30)
last_prediction = None

cap = cv2.VideoCapture("http://192.168.43.225:81/stream")
# cap = cv2.VideoCapture(0)

time.sleep(2)
while True:
    ret, photo = cap.read()

    results = model(photo)

    for pred in results.pred[0]:
        if pred[4] > 0.9:

            predictions.append(int(pred[5]))
            prediction = int(pred[5])

            cv2.rectangle(
                photo,
                (int(pred[0].item()), int(pred[1].item())),
                (int(pred[2].item()), int(pred[3].item())),
                (0, 255, 0),
                3,
            )

            if last_prediction != int(models[prediction].split("_")[1]):
                most_frequently_prediction = max(
                    set(predictions), key=predictions.count
                )

                # print(predictions.count(most_frequently_prediction))

                if predictions.count(most_frequently_prediction) > (20):
                    last_prediction = int(models[prediction].split("_")[1])
                    print(int(models[prediction].split("_")[1]))

                    ser.write(
                        int(models[prediction].split("_")[1]).to_bytes(1, "little")
                        + "\n".encode()
                    )

    cv2.putText(
        photo,
        (limits[int(last_prediction)] if last_prediction else ""),
        (50, 50),
        cv2.FONT_HERSHEY_SIMPLEX,
        1,
        (255, 0, 0),
        2,
        cv2.LINE_AA,
    )

    # print()

    # print(pred[0].item())

    cv2.imshow("streaming", photo)

    if cv2.waitKey(1) == ord("q"):
        break

"""class Sign:
    which = str
    area = float

    def __init__(self, which = "",  area=0):
        self.which = which
        self.area = area


sign1 = Sign()
sign2 = Sign()
sign3 = Sign    if cv2.waitKey(1) == ord("q"):
        break()
sign4 = Sign()
sign5 = Sign()

stream = cv2.VideoCapture(0)
time.sleep(0.1)
signs_dict = {0: "D", 1:"stop", 2: "left"}
signs_list = []

while True:
    ret, frame = stream.read()
    cv2.imshow("frame", frame)
    results = model(frame)

    if len(results.pred[0]) > 0:
        if results.pred[0][0][4].item() > 0.6:
            x1 = results.pred[0][0][0].item()
            y1 = results.pred[0][0][1].item()
            x2 = results.pred[0][0][2].item()
            y2 = results.pred[0][0][3].item()
            area = abs(x1 - x2) * abs(y1 - y2)
            print("area: ", area, signs_dict[int(results.pred[0][0][5])])
            signs_list.append(area)
            
    if len(signs_list) == 7:
                    print("*************", max(signs_list))
                    signs_list.clear()
                            
    if cv2.waitKey(1) == ord("q"):
        break"""
