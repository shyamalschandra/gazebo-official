for i in `seq 10`; do . tools/run_diagnostic.bash worlds/dual_pr2.world unthrottled0; done
for i in `seq 10`; do . tools/run_diagnostic.bash worlds/dual_pr2.world unthrottled2; done
for i in `seq 10`; do . tools/run_diagnostic.bash worlds/dual_pr2.world split_unthrottled0; done
for i in `seq 10`; do . tools/run_diagnostic.bash worlds/dual_pr2.world split_unthrottled2; done
for i in `seq 10`; do . tools/run_diagnostic.bash worlds/pr2.world unthrottled0; done
for i in `seq 10`; do . tools/run_diagnostic.bash worlds/pr2.world unthrottled2; done
for i in `seq 10`; do . tools/run_diagnostic.bash worlds/pr2.world split_unthrottled0; done
for i in `seq 10`; do . tools/run_diagnostic.bash worlds/pr2.world split_unthrottled2; done
#for i in `seq 10`; do . tools/run_diagnostic.bash test/worlds/revolute_joint_test.world unthrottled0; done
#for i in `seq 10`; do . tools/run_diagnostic.bash test/worlds/revolute_joint_test.world unthrottled2; done
#for i in `seq 10`; do . tools/run_diagnostic.bash test/worlds/revolute_joint_test.world unthrottled3; done
#for i in `seq 10`; do . tools/run_diagnostic.bash test/worlds/revolute_joint_test.world unthrottled4; done
#for i in `seq 10`; do . tools/run_diagnostic.bash test/worlds/revolute_joint_test.world split_unthrottled0; done
#for i in `seq 10`; do . tools/run_diagnostic.bash test/worlds/revolute_joint_test.world split_unthrottled2; done
#for i in `seq 10`; do . tools/run_diagnostic.bash test/worlds/revolute_joint_test.world split_unthrottled3; done
#for i in `seq 10`; do . tools/run_diagnostic.bash test/worlds/revolute_joint_test.world split_unthrottled4; done
