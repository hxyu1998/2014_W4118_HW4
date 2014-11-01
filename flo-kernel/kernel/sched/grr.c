/* new GRR schedule */
#include "sched.h"

static void dequeue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	
	struct sched_grr_entity *grr_se = &p->grr;
	/*struct rq *rq = task_rq(p);*/
	struct grr_rq *grr_rq = &rq->grr;

	spin_lock(&grr_rq->grr_rq_lock);

	/*update_curr_grr(rq);*/
	list_del(&grr_se->run_list);
	--grr_rq->grr_nr_running;

	spin_unlock(&grr_rq->grr_rq_lock);
	
}

static void enqueue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	/* maybe this can simplified 
	struct sched_grr_entity *grr_se = &p->grr;
	struct task_struct *p = container_of(grr_se, struct task_struct, rt);
	struct rq *rq = task_rq(p);
	struct grr_rq *grr_rq = rq->grr;
	struct list_head *queue = grr_rq->grr_rq_list;
	list_add(&grr_se->run_list, queue);
	grr_rq->nr_running++; 	
	*/

	struct sched_grr_entity *grr_se = &p->grr;
	struct grr_rq *grr_rq = &rq->grr;
	struct list_head *queue = &grr_rq->grr_rq_list;
	list_add(&grr_se->run_list, queue);
	grr_rq->grr_nr_running++;	
}

static void yield_task_grr(struct rq *rq)
{
}

static struct task_struct *pick_next_task_grr(struct rq *rq, struct task_struct *prev)
{
	struct list_head *temp = prev->grr.run_list.next;
	struct sched_grr_entity *tmp = list_entry(temp, struct sched_grr_entity, run_list);
	return container_of(tmp, struct task_struct, grr);
}


/* Copy from update_curr_rt in rt.c, remove what are rt particulat parts.*/
/*static void update_curr_grr(sturct rq *rq)
{
	struct task_struct *curr = rt->curr;
	struct sched_grr_entity *grr_se = &curr->grr;
	
	u64 delta_exec;
	
	if (curr->sched_class != &grr_sched_class)
		return;

	delta_exec = rq->clock_task - curr->sched_start;
	if(unlikely((s64)delta_exec < 0))
		delta_exec = 0;

	schedstat_set(curr->se.statistics.exec_max,
			max(curr->se.statistics.exec_max, delta_exec));

	curr->se.sum_exec_runtime += delta_exec;
	account_group_exec_runtime(curr, delta_exec);

	curr->se.exec_start = rq->clock_task;
	cpuacct_charge(curr, delta_exec);
}*/

static void put_prev_task_grr(struct rq *rq, struct task_struct *prev)
{
}

static void task_tick_grr(struct rq *rq, struct task_struct *p, int queued)
{
	struct sched_grr_entity *grr_se = &p->grr;

	//	update_curr_rt(grr);
		watchdog(rq, p);

		/*
		* RR tasks need a special form of timeslice management.
		* FIFO tasks have no timeslices.
		*/
	
	if (p->policy != SCHED_RR)
		return;

	if (--p->grr.time_slice)
		return;

		p->grr.time_slice = sched_rr_timeslice;

		/*	
		* Requeue to the end of queue if we (and all of our ancestors) are not
		* the only element on the queue
		*/

	for_each_sched_grr_entity(grr_se) {
		if (grr_se->run_list.prev != grr_se->run_list.next) {
			requeue_task_grr(rq, p, 0);
			set_tsk_need_resched(p);
			return;
		}
	}
}

static void set_curr_task_grr(struct rq *rq)
{
}

static void check_preempt_curr_grr(struct rq *rq,
				struct task_struct *p, int flags)
{
}

static void switched_to_grr(struct rq *rq, struct task_struct *p)
{
}

static void prio_changed_grr(struct rq *rq, struct task_struct *p, int old)
{
}

static unsigned int get_rr_interval_grr(struct rq *rq, struct task_struct *t)
{
}

const struct sched_class grr_sched_class = {
	.next			= &fair_sched_class,
	.dequeue_task		= dequeue_task_grr,
	.enqueue_task		= enqueue_task_grr,
	.yield_task		= yield_task_grr,
	.check_preempt_curr	= check_preempt_curr_grr,
	.pick_next_task		= pick_next_task_grr,
	.put_prev_task		= put_prev_task_grr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_grr,
	.pre_schedule		= pre_schedule_grr,
	.post_schedule		= post_schedule_grr,
#endif

	.set_curr_task		= set_curr_task_grr,
	.task_tick		= task_tick_grr,
	
	.get_rr_interval	= get_rr_interval_grr,

	.prio_changed		= prio_changed_grr,
	.switched_to		= switched_to_grr,
};
