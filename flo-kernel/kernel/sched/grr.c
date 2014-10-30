/* new GRR schedule */
#include <linux/sched.h>
#include <linux/linkage.h>
#include <linux/list.h>



static DEFINE_SPINLOCK()

static void dequeue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_grr_entity *grr_se = &p->grr_se;
	struct rq *rq = task_rq(p);
	struct grr_rq *grr_rq = &rq->grr;

	spin_lock(&grr_rq->grr_rq_lock);

	list_del(&grr_se->run_list);
	--grr_rq->grr_nr_running;

	spin_unlock(&grr_rq->grr_rq_lock);
}

static void enqueue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_grr_entity *grr_se = &p->grr_se;
	/*
	list_add(&grr_se->run_list, 		
	*/
}

static void yield_task_grr(struct rq *rq)
{
}

static void check_preempt_curr_grr(struct rq *rq,
				struct task_struct *p, int flags)
{
}

static struct task_struct *pick_next_task_grr(struct rq *rq, struct task_struct *prev)
{
	struct list_head *temp = prev->sched_grr_entity.run_list.next;
	struct sched_grr_entity *tmp = list_entry(temp, struct sched_grr_entiy, run_list);
	return container_of(tmp, struct task_struct, grr);
}


/* Copy from update_curr_rt in rt.c, remove what are rt particulat parts.*/
static void update_curr_grr(sturct rq *rq)
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
}

static void put_prev_task_grr(struct rq *rq, struct task_struct *prev)
{
	update_curr_grr(rq);
}

static void task_tick_grr(struct rq *rq, struct task_struct *curr, int queued)
{
}

static void set_curr_task_grr(struct rq *rq)
{
}

static void check_preempt_curr_grr(struct rq *rq,
				struct task_struct *p, int flags)
{
}

static void switch_to_grr(struct rq *rq, struct task_struct *p)
{
}

static void prio_changed_grr(struct rq *rq, struct task_struct *p, int old)
{
}

static unsigned int get_rr_interval_grr(struct rq *rq, struct task_struct *t)
{
}

static const struct sched_class grr_sched_class = {
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
