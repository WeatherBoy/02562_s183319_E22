
import matplotlib.pyplot as plt

rendering_times = [0.076, 0.184, 0.329, 0.553, 0.627, 0.953, 1.219, 1.395, 1.687, 2.055, 2.463, 2.985, 3.266]
#print(len(rendering_times))
# print(relation)

relation_squared = [rt/((i + 1)**2) for i, rt in enumerate(rendering_times)]
relation_squared_mean = [sum(relation_squared)/len(relation_squared)] * len(relation_squared)
# print(relation_squared)

fig, ax = plt.subplots(1,2)
ax[0].plot(rendering_times, label = "rendering times")
ax[0].set_title(f"Time to render in seconds")
ax[0].set_ylabel("Rendering times in seconds")
ax[0].set_xlabel("Subdivision level (sl)")

ax[1].plot(relation_squared, label = "relation_squared")
ax[1].plot(relation_squared_mean, label = "relation_squared mean")
ax[1].set_title(f"Relation_squared mean is: \n{round(relation_squared_mean[0], 4)}")
ax[1].set_ylabel("render time divided by sl squared")
ax[1].set_xlabel("Subdivision level (sl)")
plt.show()
